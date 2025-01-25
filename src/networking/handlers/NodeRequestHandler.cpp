#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
	delete hgrh;
	delete dcrh;
	delete rkerh;
	delete ekerh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, SOCKET cs, std::map<unsigned int, uint256_t>& aesKeys) :
	_circuitData(circuits), _rsaKeys(rsaKeys), _aesKeys(aesKeys), _socket(cs)
{
	this->lrh = new LinkRequestHandler(_circuitData, _socket);
	this->hgrh = new HttpGetRequestHandler(_circuitData, _socket);
	this->dcrh = new DeleteCircuitRequestHandler(_circuitData, _socket);
	this->rkerh = new RsaKeyExchangeRequestHandler(_circuitData, _socket, _rsaKeys);
	this->ekerh = new EcdheKeyExchangeRequestHandler(_circuitData, _socket, _rsaKeys, _aesKeys);
}

RequestResult NodeRequestHandler::handleMsg(RequestInfo& requestInfo, bool& isRSA)
{
	if (isRSA)
	{
		//circuit id from previus request
		std::cout << "[NODE REQUEST HANDLER] circuit id: " << requestInfo.circuit_id << "\n";
		requestInfo = Helper::waitForResponse_RSA(_socket, this->_rsaKeys[requestInfo.circuit_id].first);
		isRSA = false;
	}
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
	else if (ekerh->isRequestRelevant(requestInfo))
	{
		return ekerh->handleRequest(requestInfo);
	}
	else
	{
		throw std::runtime_error("no request much this code Node request Handler");
	}
	return RequestResult();
}