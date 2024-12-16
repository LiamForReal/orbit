#pragma once 
#include "LinkRequestHandler.h"
#include "HttpGetRequestHandler.h"
#include "DeleteCircuitRequestHandler.h"
class NodeRequestHandler 
{
public: 
	~NodeRequestHandler();
	NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, SOCKET cs);
	RequestResult directMsg(const RequestInfo& requestInfo);
private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData;
	LinkRequestHandler* lrh;
	DeleteCircuitRequestHandler* dcrh;
	HttpGetRequestHandler* hgrh;
	
	SOCKET _socket;
};