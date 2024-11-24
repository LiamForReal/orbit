#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits) : circuitData(circuits)
{
	this->lrh = new LinkRequestHandler(circuitData);
}

RequestResult NodeRequestHandler::directMsg(const RequestInfo& requestInfo)
{
	if (lrh->isRequestRelevant(requestInfo))
	{
		return lrh->handleRequest(requestInfo);
	}//more...
	else
	{
		throw std::runtime_error("no request much this code Node request Handler");
	}
	return RequestResult();
}