#include "HttpGetRequestHandler.h"
#include <fstream>

HttpGetRequestHandler::HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData) : circuitsData(circuitsData)
{
	this->rr = RequestResult();
	this->circuitsData = circuitsData;
}

bool HttpGetRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == HTTP_MSG_RC;
}

std::string HttpGetRequestHandler::sendHttpRequest(const std::string& httpRequest) 
{
    /*
    const int PORT = 80; // HTTP port
    const int BUFFER_SIZE = 4096;

    // Step 1: Extract Host from the HTTP request
    size_t hostStart = httpRequest.find("Host: ");
    if (hostStart == std::string::npos) {
        throw std::runtime_error("Host header not found in HTTP request");
    }
    hostStart += 6; // Move past "Host: "
    size_t hostEnd = httpRequest.find("\r\n", hostStart);
    std::string host = httpRequest.substr(hostStart, hostEnd - hostStart);

    // Step 2: Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    // Step 3: Resolve the domain name to an IP address
    struct hostent* hostEntry = gethostbyname(host.c_str());
    if (hostEntry == nullptr) {
        WSACleanup();
        throw std::runtime_error("Failed to resolve domain: " + host);
    }

    // Step 4: Create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    // Step 5: Set up the server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    memcpy(&serverAddr.sin_addr, hostEntry->h_addr, hostEntry->h_length);

    // Step 6: Connect to the server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        throw std::runtime_error("Failed to connect to server");
    }

    // Step 7: Send the HTTP request
    if (send(sock, httpRequest.c_str(), httpRequest.length(), 0) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        throw std::runtime_error("Failed to send request");
    }

    // Step 8: Receive the response
    char buffer[BUFFER_SIZE];
    std::string response;
    int bytesReceived;

    while ((bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        response += buffer;
    }

    if (bytesReceived == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        throw std::runtime_error("Failed to receive response");
    }

    // Step 9: Clean up
    closesocket(sock);
    WSACleanup();

    return response;
    */
    return "";
}


        //std::string response = sendHttpGetRequest(domain);

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
