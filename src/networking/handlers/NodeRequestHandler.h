#pragma once 
#include "LinkRequestHandler.h"
#include "HttpGetRequestHandler.h"
#include "DeleteCircuitRequestHandler.h"
#include "RsaKeyExchangeRequestHandler.h"
#include "EcdheKeyExchangeRequestHandler.h"
#define STATUS_INDEX 1
#define CIRCUIT_ID_INDEX 0

class NodeRequestHandler 
{
public: 
	~NodeRequestHandler();
	NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, SOCKET cs, std::map<unsigned int, AES>& aesKeys);
	RequestResult handleMsg(RequestInfo& requestInfo);
	RequestInfo getMsg();
private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitData;
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& _rsaKeys;
	std::map<unsigned int, AES>& _aesKeys;
	SOCKET _socket;

	bool _isRSA;
	bool _isFirstTime;
	bool _isAES;

	RequestInfo ri;

	LinkRequestHandler* lrh;
	DeleteCircuitRequestHandler* dcrh;
	HttpGetRequestHandler* hgrh;
	RsaKeyExchangeRequestHandler* rkerh;
	EcdheKeyExchangeRequestHandler* ekerh;
	
};