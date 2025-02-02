#include "LinkRequestHandler.h"
#define MAX_DATA_LENGTH 70000

LinkRequestHandler::LinkRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, AES>& aesKeys) 
	: _circuitData(circuitData), _socket(s), _aesKeys(aesKeys)
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
	 
	// Set buffer size before connecting
	int bufferSize = MAX_DATA_LENGTH;

	if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize)) == SOCKET_ERROR) {
		std::cerr << "Failed to set receive buffer size: " << WSAGetLastError() << std::endl;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, sizeof(bufferSize)) == SOCKET_ERROR) {
		std::cerr << "Failed to set send buffer size: " << WSAGetLastError() << std::endl;
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
	RequestInfo ri;
	unsigned int status = LINK_STATUS;
	rr.buffer.clear();
	try
	{
		unsigned int circuit_id = requestInfo.circuit_id;

		if (_circuitData[circuit_id].first == _socket)
		{
			if (_circuitData[circuit_id].second != INVALID_SOCKET && _circuitData[circuit_id].second != NULL)
			{
				std::cout << "[LINK] seconed is exisist and trying to connenct" << std::endl;
				rr.buffer = Helper::buildRR(requestInfo);
				Helper::sendVector(_circuitData[circuit_id].second, rr.buffer);
				ri = Helper::waitForResponse(_circuitData[circuit_id].second);//sends rr but I put that on ri
				//becouse ri contains only status and circuit id there is no need for encription
				rr.buffer = Helper::buildRR(ri);
				std::cout << "[LINK] sending backwards!\n";
				Helper::sendVector(_circuitData[circuit_id].first, rr.buffer);
			}
			else
			{
				LinkRequest lr = DeserializerRequests::deserializeLinkRequest(requestInfo);
				std::cout << "[LINK] seconed is new and now generating\n";
				_circuitData[requestInfo.circuit_id].second = this->createSocket(lr.nextNode.first, lr.nextNode.second);
				if (_circuitData[requestInfo.circuit_id].second == INVALID_SOCKET)
					throw std::runtime_error("[LINK] socket creation failed");
				std::cout << "[LINK] next created\n";
				rr.buffer.clear();
				//becouse ri contains only status and circuit id there is no need for encription
				rr.buffer = Helper::buildRR(status ,circuit_id);
				std::cout << "[LINK] sending backwards!\n";
				Helper::sendVector(_circuitData[circuit_id].first, rr.buffer);
			}
		}
		else throw std::runtime_error("the socket given is corrupted");

	}
	catch (std::runtime_error& e)
	{
		status = LINK_ERROR;
	}
	return rr;
}