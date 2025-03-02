#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
	delete hgrh;
	delete dcrh;
	delete rkerh;
	delete ekerh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, SOCKET cs, std::map<unsigned int, AES>& aesKeys) :
	_circuitData(circuits), _rsaKeys(rsaKeys), _aesKeys(aesKeys), _socket(cs)
{
	this->lrh = new LinkRequestHandler(_circuitData, _socket, _aesKeys);
	this->hgrh = new HttpGetRequestHandler(_circuitData, _aesKeys);
	this->dcrh = new DeleteCircuitRequestHandler(_circuitData, _rsaKeys, _aesKeys, _socket);
	this->rkerh = new RsaKeyExchangeRequestHandler(_circuitData, _socket, _rsaKeys, _aesKeys);
	this->ekerh = new EcdheKeyExchangeRequestHandler(_circuitData, _socket, _rsaKeys, _aesKeys);

	_isRSA = false;
	_isAES = false;
	_isFirstTime = true;
}

RequestInfo NodeRequestHandler::getMsg()
{
	if (_isRSA && _isFirstTime)
	{
		//circuit id from previus request
		std::cout << "[NODE REQUEST HANDLER] circuit id: " << ri.circuit_id << "\n";
		ri = Helper::waitForResponse_RSA(_socket, this->_rsaKeys[ri.circuit_id].first);
		_isRSA = false;
		_isFirstTime = false;
	}
	else if (_isAES)
	{
		ri = Helper::waitForResponse_AES(_socket, this->_aesKeys[ri.circuit_id], false); // decription
	}
	else
	{
		ri = Helper::waitForResponse(_socket);
	}
	return ri;
}

RequestResult NodeRequestHandler::handleMsg(RequestInfo& requestInfo)
{
	RequestResult rr;

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
		rr = rkerh->handleRequest(requestInfo);
		if (unsigned int(rr.buffer[STATUS_INDEX]) == RSA_KEY_EXCHANGE_STATUS && _isFirstTime)
			_isRSA = true;
	}
	else if (ekerh->isRequestRelevant(requestInfo))
	{
		rr = ekerh->handleRequest(requestInfo);
		if (unsigned int(rr.buffer[STATUS_INDEX]) == ECDHE_KEY_EXCHANGE_STATUS)
		{
			_isAES = true;
		}
	}
	else
	{
		throw std::runtime_error("no request much this code Node request Handler");
	}
	return RequestResult();
}