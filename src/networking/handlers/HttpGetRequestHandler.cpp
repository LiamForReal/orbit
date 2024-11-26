#include "HttpGetRequestHandler.h"
#include <stdlib.h>
#include <curl/curl.h>
#include <string>

HttpGetRequestHandler::HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData) : circuitsData(circuitsData)
{
	this->rr = RequestResult();
	this->circuitsData = circuitsData;
}

bool HttpGetRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == HTTP_MSG_RC;
}

size_t HttpGetRequestHandler::writeChunk(void* data, size_t size, size_t nmemb, void* userData)
{
	size_t real_size = size * nmemb;

	CurlResponse* curlResponse = (CurlResponse*)(userData);

	// allocating additional memory for new chunk + 1 byte for NULL terminator 
	char* pointer = (char*)(realloc(curlResponse->string, curlResponse->size + real_size + 1));

	if (NULL == pointer)
	{
		return CURL_WRITEFUNC_ERROR;
	}

	curlResponse->string = pointer;

	// appending chunk data to the end of the last chunk
	memcpy(&(curlResponse->string[curlResponse->size]), data, real_size);
	curlResponse->size += real_size;
	curlResponse->string[curlResponse->size] = NULL;

	return real_size;
}

std::string HttpGetRequestHandler::sendHttpRequest(const std::string& httpRequest)
{
	CURL* curl = curl_easy_init();
	CURLcode result;
	if (NULL == curl)
	{
		std::cout << "curl failed\n";
		return "";
	}

	CurlResponse curlResponse;
	curlResponse.string = (char*)(malloc(1));
	curlResponse.size = 0;

	curl_easy_setopt(curl, CURLOPT_URL, httpRequest.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &HttpGetRequestHandler::writeChunk);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)(&curlResponse));

	result = curl_easy_perform(curl);

	if (CURLE_OK != result)
	{
		std::cout << "curl failed: " << curl_easy_strerror(result) << std::endl;
		return "";
	}

	curl_easy_cleanup(curl);

	std::string htmlCode = std::string(curlResponse.string);

	free(curlResponse.string);

	return htmlCode;
}

RequestResult HttpGetRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	this->rr.buffer.clear();

	HttpGetRequest hgRequest;
	HttpGetResponse hgResponse;

	try
	{
		hgRequest = DeserializerRequests::deserializeHttpGetRequest(requestInfo.buffer);
		
        rr.circuit_id = hgRequest.circuit_id;
		// check if there is next
		if (this->circuitsData[hgRequest.circuit_id].second != INVALID_SOCKET)
		{
			hgResponse.status = HTTP_MSG_STATUS_FOWARD;
			std::vector<unsigned char> buffer = SerializerRequests::serializeRequest(hgRequest);
			Helper::sendVector(this->circuitsData[hgRequest.circuit_id].second, buffer);
		}
		else
		{
			// send HTTP GET (hgRequest.msg) to Web Server
			hgResponse.status = HTTP_MSG_STATUS_BACKWARD;
			hgResponse.content = this->sendHttpRequest(hgRequest.msg);
		}
	}
	catch (std::runtime_error e)
	{
		hgResponse.status = Errors::HTTP_MSG_ERROR;
		hgResponse.content = "";
	}

	this->rr.buffer = SerializerResponses::serializeResponse(hgResponse);
	return this->rr;
}
