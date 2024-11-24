#pragma once
#include "IRequestHandler.h" 

class HttpGetRequestHandler : virtual public IRequestHandler
{
	public:
		HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData, SOCKET& socket);
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		RequestResult handleRequest(const RequestInfo& requestInfo) override;

	private:
		RequestResult rr;
		SOCKET& _socket;
		std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData;
};