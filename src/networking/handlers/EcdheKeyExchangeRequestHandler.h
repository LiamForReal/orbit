#pragma once
#include "IRequestHandler.h"
#include "../../utils/ECDHE.h"

class EcdheKeyExchangeRequestHandler : virtual public IRequestHandler
{
public:
	EcdheKeyExchangeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, std::map<unsigned int, AES>& aesKeys);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitData;
	SOCKET& _socket;								
	std::map<unsigned int, ECDHE> _ecdheInfo; //circuit id to pair of ecdhe main object and seconed only a saver of tmpKey 
												   //public key, product
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& _rsaKeys; // circuit id to pair of: rsa self decriptor, rsa properties to encript
	std::map<unsigned int, AES>& _aesKeys; // circuit id to aes keys 
	RequestResult rr;
};