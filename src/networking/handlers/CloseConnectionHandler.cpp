#include "CloseConnectionHandler.h"

CloseConnectionHandler::CloseConnectionHandler(std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>&controlList, unsigned int& circuitId)
	: _controlList(controlList), _circuitId(circuitId)
{
	if (_circuitId == 0)
		throw std::runtime_error("circuit id supppose to be filled in this stage");
	rr = RequestResult();
}

bool CloseConnectionHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == CLOSE_CONNECTION_RC;
}

SOCKET CloseConnectionHandler::createSocket(const std::string& ip, unsigned int port)
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

	int flag = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

	// Connect to the server
	if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}

void CloseConnectionHandler::sendDeleteToNode(SOCKET node_sock)
{
	Helper::sendVector(node_sock, rr.buffer);
	RequestInfo ri = Helper::waitForResponse(node_sock);
	if (ri.id != DELETE_CIRCUIT_STATUS)
		throw std::runtime_error("node delete failed!");
}

RequestResult CloseConnectionHandler::handleRequest(RequestInfo& requestInfo)
{
	unsigned int status = CLOSE_CONNECTION_STATUS;
	rr.buffer.clear();
	try
	{
		rr.buffer = Helper::buildRR(DELETE_CIRCUIT_RC, _circuitId);
		std::vector<std::thread> nodeSender;
		for (auto it : _controlList[_circuitId])
		{
			nodeSender.emplace_back(std::thread(&CloseConnectionHandler::sendDeleteToNode,
												this,
												createSocket(it.first, unsigned int(std::stoi(it.second)))));
		}

		for (auto it = nodeSender.begin(); it != nodeSender.end(); ++it)
		{
			it->join();
		}
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
		status = CLOSE_CONNECTION_ERROR;
	}
	catch (...)
	{
		status = CLOSE_CONNECTION_ERROR;
	}
	rr.buffer = Helper::buildRR(status, _circuitId);
	return rr;
}
