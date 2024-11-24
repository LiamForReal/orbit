#pragma once
#include "IRequestHandler.h"
#include <ws2tcpip.h>

class LinkRequestHandler : virtual public IRequestHandler
{
public:
	LinkRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:
	SOCKET createSocket(const std::string& ip, unsigned int port);
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& cd;
	RequestResult rr;
};