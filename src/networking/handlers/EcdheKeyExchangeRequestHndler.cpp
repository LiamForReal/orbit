#include "EcdheKeyExchangeRequestHndler.h"

EcdheKeyExchangeRequestHndler::EcdheKeyExchangeRequestHndler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaCircuits)
	:_circuitData(circuitData), _socket(s), _rsaCircuits(rsaCircuits)
{
	this->rr = RequestResult();
}

bool EcdheKeyExchangeRequestHndler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == ECDHE_KEY_EXCHANGE_RC;
}

RequestResult EcdheKeyExchangeRequestHndler::handleRequest(const RequestInfo& requestInfo)
{
	//TODO
	return RequestResult();
}