#include "NodeOpeningHandler.h"

bool NodeOpeningHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return false;
}

RequestResult NodeOpeningHandler::handleRequest(const RequestInfo& requestInfo)
{
	return RequestResult();
}
