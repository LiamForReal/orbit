#pragma once

#include "IRequestHandler.h"
#include <ws2tcpip.h>

class CloseConnectionHandler : public IRequestHandler
{
public:
	CloseConnectionHandler(std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, unsigned int& circuitId);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(RequestInfo& requestInfo) override;
private:
	SOCKET createSocket(const std::string& ip, unsigned int port);
	void sendDeleteToNode(SOCKET node_sock);

	RequestResult rr;
	std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList;
	unsigned int& _circuitId;
};