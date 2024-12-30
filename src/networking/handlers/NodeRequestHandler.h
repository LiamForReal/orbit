#pragma once 
#include "LinkRequestHandler.h"
#include "HttpGetRequestHandler.h"
#include "DeleteCircuitRequestHandler.h"
class NodeRequestHandler 
{
public: 
	~NodeRequestHandler();
	NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaCircuits, SOCKET cs);
	RequestResult directMsg(const RequestInfo& requestInfo);
private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData;
	std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaCircuits;
	LinkRequestHandler* lrh;
	DeleteCircuitRequestHandler* dcrh;
	HttpGetRequestHandler* hgrh;
	
	SOCKET _socket;
};