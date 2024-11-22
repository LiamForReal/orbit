#include "TorRequestHandler.h"

TorRequestHandler::TorRequestHandler(DockerManager& newDm) : dm(newDm)
{
	noh = new NodeOpeningHandler();
	//more...
}

RequestResult TorRequestHandler::directRequest(const RequestInfo& requestInfo)
{
	if (noh->isRequestRelevant(requestInfo))
	{
		return noh->handleRequest(requestInfo, dm);
	}//more...
	else
	{
		throw std::runtime_error("no request much this code");
	}
}