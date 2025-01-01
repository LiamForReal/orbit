#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
	delete hgrh;
	delete dcrh;
	delete rkerh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaCircuits, SOCKET cs) :
	_circuitData(circuits), _rsaCircuits(rsaCircuits), _socket(cs)
{
	this->lrh = new LinkRequestHandler(_circuitData, _socket);
	this->hgrh = new HttpGetRequestHandler(_circuitData, _socket);
	this->dcrh = new DeleteCircuitRequestHandler(_circuitData, _socket);
	this->rkerh = new RsaKeyExchangeRequestHandler(_circuitData, _socket, _rsaCircuits);
}

RequestResult NodeRequestHandler::handleMsg(const RequestInfo& requestInfo)
{
	if (lrh->isRequestRelevant(requestInfo))
	{
		return lrh->handleRequest(requestInfo);
	}
	else if (hgrh->isRequestRelevant(requestInfo))
	{
		return hgrh->handleRequest(requestInfo);
	}
	else if (dcrh->isRequestRelevant(requestInfo))
	{
		return dcrh->handleRequest(requestInfo);
	}
	else if (rkerh->isRequestRelevant(requestInfo))
	{
		return rkerh->handleRequest(requestInfo);
	}
	else
	{
		throw std::runtime_error("no request much this code Node request Handler");
	}
	return RequestResult();
}