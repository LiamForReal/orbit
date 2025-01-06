#pragma once 
#include "LinkRequestHandler.h"
#include "HttpGetRequestHandler.h"
#include "DeleteCircuitRequestHandler.h"
#include "RsaKeyExchangeRequestHandler.h"
class NodeRequestHandler 
{
public: 
	~NodeRequestHandler();
	NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaCircuits, SOCKET cs);
	RequestResult handleMsg(const RequestInfo& requestInfo);
private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitData;
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& _rsaCircuits;
	SOCKET _socket;

	LinkRequestHandler* lrh;
	DeleteCircuitRequestHandler* dcrh;
	HttpGetRequestHandler* hgrh;
	RsaKeyExchangeRequestHandler* rkerh;
	
	
};