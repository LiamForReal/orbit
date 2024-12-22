#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
	delete hgrh;
	delete dcrh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, SOCKET cs) : circuitData(circuits), _socket(cs)
{
	this->lrh = new LinkRequestHandler(circuitData, _socket);
	this->hgrh = new HttpGetRequestHandler(circuitData, _socket);
	dcrh = new DeleteCircuitRequestHandler(circuitData, _socket);
}

RequestResult NodeRequestHandler::directMsg(const RequestInfo& requestInfo)
{
	if (lrh->isRequestRelevant(requestInfo))
	{
		return lrh->handleRequest(requestInfo);
	}
	else if (hgrh->isRequestRelevant(requestInfo))
	{
		return hgrh->handleRequest(requestInfo);
	}
	else if (dcrh->isRequestRelevant(requestInfo))
	{
		return dcrh->handleRequest(requestInfo);
	}
	else
	{
		throw std::runtime_error("no request much this code Node request Handler");
	}
	return RequestResult();
}