#pragma once
#include "IRequestHandler.h"
#include "../../utils/ECDHE.h"

class EcdheKeyExchangeRequestHandler : virtual public IRequestHandler
{
public:
	EcdheKeyExchangeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, std::map<unsigned int, uint256_t>& aesKeys);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitData;
	SOCKET& _socket;								//public, product
	std::map<unsigned int, std::pair<ECDHE, uint256_t>> _ecdheInfo;
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& _rsaKeys;
	std::map<unsigned int, uint256_t>& _aesKeys;
	RequestResult rr;
};