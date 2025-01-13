#include "LinkRequestHandler.h"

LinkRequestHandler::LinkRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s) : _circuitData(circuitData), _socket(s)
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
	rr.buffer.clear();
	LinkRequest lr;
	LinkResponse lre;
	RequestInfo ri;
	try
	{
		lr = DeserializerRequests::deserializeLinkRequest(requestInfo.buffer);

		rr.circuit_id = requestInfo.circuit_id;

		lre.status = LINK_STATUS;

		if (_circuitData[requestInfo.circuit_id].first == _socket)
		{
			if (_circuitData[requestInfo.circuit_id].second != INVALID_SOCKET && _circuitData[requestInfo.circuit_id].second != NULL)
			{
				std::cout << "[LINK] seconed is exisist and trying to connenct" << std::endl;
				Helper::sendVector(_circuitData[requestInfo.circuit_id].second, requestInfo.buffer);
				ri = Helper::waitForResponse(_circuitData[requestInfo.circuit_id].second);//sends rr but I put that on ri
				if (ri.id == LINK_STATUS)
					lre.status = LINK_STATUS;
				else throw std::runtime_error("[LINK] problem occurred while linking the next node");
				std::cout << "[LINK] sends to the next node!\n";
				rr.buffer = SerializerResponses::serializeResponse(lre);
				std::cout << "[LINK] sending backwards!\n";
				Helper::sendVector(_circuitData[rr.circuit_id].first, rr.buffer);
			}
			else
			{
				std::cout << "[LINK] seconed is new and now generating\n";
				_circuitData[requestInfo.circuit_id].second = this->createSocket(lr.nextNode.first, lr.nextNode.second);
				if (_circuitData[requestInfo.circuit_id].second == INVALID_SOCKET)
					throw std::runtime_error("[LINK] socket creation failed");
				std::cout << "[LINK] next created";
				rr.buffer = SerializerResponses::serializeResponse(lre);
				std::cout << "[LINK] sending backwards!\n";
				Helper::sendVector(_circuitData[rr.circuit_id].first, rr.buffer);
			}
		}
		else throw std::runtime_error("the socket given is corrupted");

	}
	catch (std::runtime_error& e)
	{
		lre.status = LINK_ERROR;
	}
	return rr;
}