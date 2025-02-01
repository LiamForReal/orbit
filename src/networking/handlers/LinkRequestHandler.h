#pragma once
#include "IRequestHandler.h"
#include <ws2tcpip.h>

class LinkRequestHandler : virtual public IRequestHandler
{
public:
	LinkRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, AES>& aesKeys);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:
	SOCKET createSocket(const std::string& ip, unsigned int port);
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitData;
	SOCKET& _socket;
	std::map<unsigned int, AES>& _aesKeys;
	RequestResult rr;
};