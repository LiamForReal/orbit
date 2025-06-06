#include "HttpGetRequestHandler.h"
#include <stdlib.h>
#include <curl/curl.h>
#include <string>

HttpGetRequestHandler::HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData, std::map<unsigned int, AES>& aesKeys)
    : _circuitsData(circuitsData), _aesKeys(aesKeys)
{
    this->rr = RequestResult();
}

bool HttpGetRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
    return requestInfo.id == HTTP_MSG_RC;
}

size_t HttpGetRequestHandler::writeChunk(void* data, size_t size, size_t nmemb, void* userData)
{
    size_t real_size = size * nmemb;

    CurlResponse* curlResponse = static_cast<CurlResponse*>(userData);

    // Reallocate memory for the new data chunk + null terminator
    char* pointer = static_cast<char*>(realloc(curlResponse->string, curlResponse->size + real_size + 1));
    if (pointer == NULL)
    {
        std::cerr << "Memory allocation failed!" << std::endl;
        return 0; // Return 0 to indicate failure to libcurl
    }

    curlResponse->string = pointer;

    // Copy the new data chunk to the reallocated memory
    memcpy(&(curlResponse->string[curlResponse->size]), data, real_size);
    curlResponse->size += real_size;

    // Null-terminate the string
    curlResponse->string[curlResponse->size] = '\0';

    return real_size;
}

std::string HttpGetRequestHandler::sendHttpRequest(const std::string& httpRequest)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL!" << std::endl;
        return "";
    }

    // Initialize CurlResponse
    CurlResponse curlResponse;
    curlResponse.string = static_cast<char*>(malloc(1)); // Allocate initial memory
    if (!curlResponse.string)
    {
        std::cerr << "Memory allocation failed!" << std::endl;
        curl_easy_cleanup(curl);
        return "";
    }
    curlResponse.size = 0;

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, httpRequest.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpGetRequestHandler::writeChunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlResponse);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "./cacert.pem");

    // Perform the HTTP request
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK)
    {
        std::cerr << "CURL failed: " << curl_easy_strerror(result) << std::endl;
        free(curlResponse.string);
        curl_easy_cleanup(curl);
        return "";
    }

    // Cleanup CURL
    curl_easy_cleanup(curl);

    // Convert the response to std::string
    std::string htmlCode(curlResponse.string);

    // Free allocated memory
    free(curlResponse.string);

    return htmlCode;
}

RequestResult HttpGetRequestHandler::handleRequest(RequestInfo& requestInfo)
{
    this->rr.buffer.clear();
    HttpGetResponse hgResponse;
    RequestInfo ri;
    unsigned int status = HTTP_MSG_STATUS;
    try
    {
        
        unsigned int circuit_id = requestInfo.circuit_id;
        // check if there is next
        if (_circuitsData[circuit_id].second != INVALID_SOCKET && _circuitsData[circuit_id].second != NULL)
        {
            rr.buffer = Helper::buildRR(requestInfo);
            Helper::sendVector(_circuitsData[circuit_id].second, rr.buffer);
            std::cout << "[HTTP GET] listening forward\n";
            ri = Helper::waitForResponse(_circuitsData[circuit_id].second);
            std::cout << "[HTTP GET] msg encript by this aes layer\n";
            ri.buffer = _aesKeys[circuit_id].encrypt(ri.buffer);
            std::cout << "[HTTP GET] sending backwards!\n";
            rr.buffer = Helper::buildRR(ri);
            Helper::sendVector(_circuitsData[circuit_id].first, rr.buffer);
        }
        else
        {
            HttpGetRequest hgRequest = DeserializerRequests::deserializeHttpGetRequest(requestInfo);
            //_circuitsData[requestInfo.circuit_id].first = _socket;
            // send HTTP GET (hgRequest.msg) to Web Server
            hgResponse.content = this->sendHttpRequest(hgRequest.domain);
            std::cout << "[HTTP GET] sending backwards!\n";

            std::vector<unsigned char> data = SerializerResponses::serializeResponse(hgResponse);
            std::cout << "[HTTP GET] msg encript by this aes layer\n";
            data = _aesKeys[circuit_id].encrypt(data);
            rr.buffer = Helper::buildRR(data, status, data.size(), circuit_id);
            Helper::sendVector(_circuitsData[circuit_id].first, rr.buffer);
        }
    }
    catch (std::runtime_error e)
    {
        status = Errors::HTTP_MSG_ERROR;
        hgResponse.content = "";
    }
    catch (...)
    {
        std::cout << "[HTTP GET] unecpected error!!!\n";
        throw;
    }
    return this->rr;
}
