#include "LinkRequestHandler.h"

LinkRequestHandler::LinkRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s) : cd(circuitData), _socket(s)
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
		if (this->cd.find(lr.circuit_id) == cd.end())
		{
			cd[lr.circuit_id].first = _socket;
		}

		lre.status = LINK_STATUS;

		if (cd[lr.circuit_id].first == _socket)
		{
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
		else if (cd[lr.circuit_id].second == _socket)
		{
			Helper::sendVector(cd[lr.circuit_id].first, requestInfo.buffer);
			std::cout << "sends to the prev node!\n";
		}
		else throw std::runtime_error("the socket given is corrupted");
		
	}
	catch (std::runtime_error& e)
	{
		lre.status = LINK_ERROR;
	}

	rr.buffer = SerializerResponses::serializeResponse(lre);
	rr.circuit_id = lr.circuit_id;
	return rr;
}