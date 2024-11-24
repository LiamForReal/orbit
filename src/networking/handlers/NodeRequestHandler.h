#pragma once 
#include "LinkRequestHandler.h"

class NodeRequestHandler
{
public: 
	~NodeRequestHandler();
	NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits);
	RequestResult directMsg(const RequestInfo& requestInfo);
private:
	std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData;
	LinkRequestHandler* lrh;
};