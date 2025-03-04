#include "TorRequestHandler.h"


TorRequestHandler::~TorRequestHandler()
{
	delete noh;
}

TorRequestHandler::TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, std::map<unsigned int, SOCKET>& clients, AES& aes)
	: dm(newDm), _controlList(controlList), _clients(clients), _aes(aes)
{
	noh = new NodeOpeningHandler(dm, _controlList, _clients, _aes);
	//more...
}

RequestResult TorRequestHandler::directRequest(RequestInfo& requestInfo)
{
	if (noh->isRequestRelevant(requestInfo))
	{
		//std::cout << "\n\n\nIm here1\n\n\n";
		return noh->handleRequest(requestInfo);
	}//more...
	else
	{
		throw std::runtime_error("no request much this code Tor request handler");
	}
	return RequestResult();
}