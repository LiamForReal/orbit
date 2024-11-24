#include "LinkRequestHandler.h"

LinkRequestHandler::LinkRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData) : cd(circuitData)
{
	this->rr = RequestResult(); 
}

SOCKET LinkRequestHandler::createSocket(const std::string& ip, unsigned int port)
{
	SOCKET sock = INVALID_SOCKET;

	// Create the socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
		return INVALID_SOCKET;
	}

	// Set up the sockaddr_in structure
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port); // Convert to network byte order
	if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
		std::cerr << "Invalid IP address format" << std::endl;
		closesocket(sock);
		return INVALID_SOCKET;
	}

	// Connect to the server
	if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}

bool LinkRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == LINK_RC;
}

RequestResult LinkRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	this->rr.buffer.clear();
	LinkRequest lr;
	LinkResponse lre;
	try
	{
		lr = DeserializerRequests::deserializeLinkRequest(requestInfo.buffer);
		lre.status = LINK_STATUS;
		if (cd[lr.circuit_id].second != INVALID_SOCKET)
		{
			Helper::sendVector(cd[lr.circuit_id].second, requestInfo.buffer);
			std::cout << "sends to the next node!\n";
		}
		else
		{
			cd[lr.circuit_id].second = this->createSocket(lr.nextNode.first, lr.nextNode.second);
			if (cd[lr.circuit_id].second == INVALID_SOCKET)
				throw std::runtime_error("socket creation failed");
			std::cout << "next created";
		}
	}
	catch (std::runtime_error& e)
	{
		lre.status = LINK_ERROR;
	}
	rr.buffer = SerializerResponses::serializeResponse(lre);
	return rr;
}