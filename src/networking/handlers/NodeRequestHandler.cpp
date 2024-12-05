#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
	delete hgrh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, SOCKET cs) : circuitData(circuits), client_socket(cs)
{
	this->lrh = new LinkRequestHandler(circuitData, client_socket);
	this->hgrh = new HttpGetRequestHandler(circuitData);
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
	else
	{
		throw std::runtime_error("no request much this code Node request Handler");
	}
	return RequestResult();
}