#pragma comment(lib, "Ws2_32.lib")
#include "IRequestHandler.h"

class DeleteCircuitRequestHandler : virtual public IRequestHandler
{
public:
	DeleteCircuitRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, std::map<unsigned int, AES>& aesKeys, SOCKET& socket);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(RequestInfo& requestInfo) override;

private:
	RequestResult rr;
	void closeSocket(SOCKET sockToClose);

	SOCKET& _socket;
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& cd;
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& _rsaKeys;
	std::map<unsigned int, AES>& _aesKeys;
};