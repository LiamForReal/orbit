#include "TorRequestHandler.h"


TorRequestHandler::~TorRequestHandler()
{
	delete noh;
}

TorRequestHandler::TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::list<std::pair<std::string, std::string>>>& controlList) : dm(newDm), _controlList(controlList)
{
	noh = new NodeOpeningHandler(dm, _controlList);
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