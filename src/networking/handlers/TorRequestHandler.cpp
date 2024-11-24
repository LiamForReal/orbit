#include "TorRequestHandler.h"


TorRequestHandler::~TorRequestHandler()
{
	delete noh;
}

TorRequestHandler::TorRequestHandler(DockerManager& newDm) : dm(newDm)
{
	noh = new NodeOpeningHandler(dm);
	//more...
}

RequestResult TorRequestHandler::directRequest(const RequestInfo& requestInfo)
{
	if (noh->isRequestRelevant(requestInfo))
	{
		return noh->handleRequest(requestInfo);
	}//more...
	else
	{
		throw std::runtime_error("no request much this code Tor request handler");
	}
	return RequestResult();
}