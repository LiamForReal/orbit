#include "TorRequestHandler.h"


TorRequestHandler::~TorRequestHandler()
{
	delete noh;
	delete dcrh;
}

TorRequestHandler::TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList) 
	: dm(newDm), _controlList(controlList)
{
	circuitId = 0;
	noh = new NodeOpeningHandler(dm, _controlList);
	dcrh = new DeleteCircuitRequestHandler(dm, _controlList, circuitId);
	//more...
}

RequestResult TorRequestHandler::directRequest(const RequestInfo& requestInfo)
{
	if (noh->isRequestRelevant(requestInfo))
	{
		return noh->handleRequest(requestInfo);
	}//more...
	else if (dcrh->isRequestRelevant(requestInfo))
	{
		if (this->circuitId == 0)
			throw std::runtime_error("need to inishialize circuit id");
		return dcrh->handleRequest(requestInfo);
	}
	else
	{
		throw std::runtime_error("no request much this code Tor request handler");
	}
	return RequestResult();
}