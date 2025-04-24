#include "TorRequestHandler.h"


TorRequestHandler::~TorRequestHandler()
{
	delete noh;
	delete cch;
}

TorRequestHandler::TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, std::map<unsigned int, SOCKET>& clients, AES& aes)
	: dm(newDm), _controlList(controlList), _clients(clients), _aes(aes)
{
	noh = new NodeOpeningHandler(dm, _controlList, _clients, _aes);
	cch = nullptr;
	//more...
}

RequestResult TorRequestHandler::directRequest(RequestInfo& requestInfo)
{
	if (noh->isRequestRelevant(requestInfo))
	{
		return noh->handleRequest(requestInfo);
	}//more...
	else if (requestInfo.id == CLOSE_CONNECTION_RC)
	{
		if (requestInfo.circuit_id == 0)
			throw std::runtime_error("somting went wrong in circuit saving");

		if (cch == nullptr)
			cch = new CloseConnectionHandler(_controlList, requestInfo.circuit_id);

		return cch->handleRequest(requestInfo);
	}
	else
	{
		throw std::runtime_error("no request much this code Tor request handler");
	}
	return RequestResult();
}