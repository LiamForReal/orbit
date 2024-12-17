#include "TorRequestHandler.h"


TorRequestHandler::~TorRequestHandler()
{
	delete noh;
}

TorRequestHandler::TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, std::map<unsigned int, SOCKET>& clients)
	: dm(newDm), _controlList(controlList), _clients(clients)
{
	noh = new NodeOpeningHandler(dm, _controlList, _clients);
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