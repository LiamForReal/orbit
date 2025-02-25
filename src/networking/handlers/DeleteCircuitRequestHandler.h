#pragma comment(lib, "Ws2_32.lib")
#include "IRequestHandler.h"

class DeleteCircuitRequestHandler : virtual public IRequestHandler
{
public:
	DeleteCircuitRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(RequestInfo& requestInfo) override;

private:		
	RequestResult rr;
	void closeSocket(SOCKET sockToClose);
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& cd;
};