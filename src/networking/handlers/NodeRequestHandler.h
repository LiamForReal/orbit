#pragma once 
#include "LinkRequestHandler.h"
#include "HttpGetRequestHandler.h"
#include "DeleteCircuitRequestHandler.h"
#include "RsaKeyExchangeRequestHandler.h"
#include "EcdheKeyExchangeRequestHandler.h"

class NodeRequestHandler 
{
public: 
	~NodeRequestHandler();
	NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, SOCKET cs, std::map<unsigned int, uint256_t>& aesKeys);
	RequestResult handleMsg(RequestInfo& requestInfo, bool& isRSA);
private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitData;
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& _rsaKeys;
	std::map<unsigned int, uint256_t>& _aesKeys;
	SOCKET _socket;

	LinkRequestHandler* lrh;
	DeleteCircuitRequestHandler* dcrh;
	HttpGetRequestHandler* hgrh;
	RsaKeyExchangeRequestHandler* rkerh;
	EcdheKeyExchangeRequestHandler* ekerh;
};