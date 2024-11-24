#include "LinkRequestHandler.h"

LinkRequestHandler::LinkRequestHandler() { this->rr = RequestResult(); }

bool LinkRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == LINK_RC;
}

RequestResult LinkRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	this->rr.buffer.clear();
	LinkRequest lr;
	try
	{
		//lr.nextNode
	}
	catch (std::runtime_error& e)
	{
		
	}

	return this->rr;
}