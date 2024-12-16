#pragma comment(lib, "Ws2_32.lib")
#include "IRequestHandler.h"

class DeleteCircuitRequestHandler : virtual public IRequestHandler
{
public:
	DeleteCircuitRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData, SOCKET& clientSock);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:		
	RequestResult rr;
	void closeSocket(SOCKET sockToClose);
	SOCKET& _socket;
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& cd;
};