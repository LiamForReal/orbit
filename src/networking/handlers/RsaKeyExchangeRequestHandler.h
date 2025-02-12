#pragma once 

#include "IRequestHandler.h";

class RsaKeyExchangeRequestHandler : virtual public IRequestHandler
{
public:
	RsaKeyExchangeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, std::map<unsigned int, AES>& aesKeys);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(RequestInfo& requestInfo) override;

private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitData;
	SOCKET& _socket;
	std::map<unsigned int, AES>& _aesKeys;
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& _rsaKeys;
	RequestResult rr;
};