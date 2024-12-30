#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
	delete hgrh;
	delete dcrh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaCircuits, SOCKET cs)
{
	this->lrh = new LinkRequestHandler(circuitData, _socket);
	this->hgrh = new HttpGetRequestHandler(circuitData, _socket);
	this->dcrh = new DeleteCircuitRequestHandler(circuitData, _socket);
	this->rsaCircuits = rsaCircuits;
}

RequestResult NodeRequestHandler::directMsg(const RequestInfo& requestInfo)
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
	else if (RequestCode::RSA_KEY_EXCHANGE_RC == requestInfo.id)
	{
		RequestResult rr;
		RequestInfo ri;
		RsaKeyExchangeRequest rkeRequest;
		RsaKeyExchangeResponse rkeResponse;

		try
		{
			rkeRequest = DeserializerRequests::deserializeRsaKeyExchangeRequest(requestInfo.buffer);
			rr.circuit_id = rkeRequest.circuit_id;
			
			rsaCircuits[rr.circuit_id] = std::pair<RSA, std::pair<uint2048_t, uint2048_t>>(RSA(), std::pair<uint2048_t, uint2048_t>(rkeRequest.public_key, rkeRequest.product));

			rkeResponse.status = RSA_KEY_EXCHANGE_STATUS;
		}
		catch (std::runtime_error& e)
		{
			rkeResponse.status = RSA_KEY_EXCHANGE_ERROR;
			std::cout << e.what() << std::endl;
		}
		rr.buffer = SerializerResponses::serializeResponse(rkeResponse);
		
		return rr;
	}
	else
	{
		throw std::runtime_error("no request much this code Node request Handler");
	}
	return RequestResult();
}