#include "NodeRequestHandler.h"

NodeRequestHandler::~NodeRequestHandler()
{
	delete lrh;
	delete hgrh;
	delete dcrh;
}

NodeRequestHandler::NodeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuits, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaCircuits, SOCKET cs) :
	circuitData(circuits), rsaCircuits(rsaCircuits)
{
	this->lrh = new LinkRequestHandler(circuitData, _socket);
	this->hgrh = new HttpGetRequestHandler(circuitData, _socket);
	this->dcrh = new DeleteCircuitRequestHandler(circuitData, _socket);
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
			
			rkeResponse.status = RSA_KEY_EXCHANGE_STATUS;

			if (circuitData[rr.circuit_id].first == _socket)
			{
				if (circuitData[rr.circuit_id].second != INVALID_SOCKET && circuitData[rr.circuit_id].second != NULL)
				{
					// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NEXT
					// SEND TO NEXT AND WAIT FOR RESPONSE AND THEN SEND BACKWARDS
					std::cout << "[RSA] Got from prev, there is next, sending and listening forward\n";
					Helper::sendVector(circuitData[rr.circuit_id].second, requestInfo.buffer);
					ri = Helper::waitForResponse(circuitData[rr.circuit_id].second);//sends rr but I put that on ri
					std::cout << "[RSA] sending backwards\n";
					Helper::sendVector(circuitData[rr.circuit_id].first, ri.buffer);
				}
				else
				{
					// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NO NEXT
					// SAVE RSA AND SEND BACKWARDS
					std::cout << "[RSA] Got from prev, there is no next, saving and sending backwards\n";
					// TODO: HERE CREATE THE RSA OF THE NODE
					RSA rsa;
					rsa.pregenerateKeys();
					rsaCircuits[rr.circuit_id] = std::pair<RSA, std::pair<uint2048_t, uint2048_t>>(rsa, std::pair<uint2048_t, uint2048_t>(rkeRequest.public_key, rkeRequest.product));
					rkeResponse.public_key = rsaCircuits[rr.circuit_id].second.first;
					rkeResponse.product = rsaCircuits[rr.circuit_id].second.second;
					Helper::sendVector(circuitData[rr.circuit_id].first, SerializerResponses::serializeResponse(rkeResponse));
				}
			}
			else if (circuitData[rr.circuit_id].second == _socket)
			{
				// RSA HANDLING IF GOT MSG FROM NEXT
				// SEND BACKWARDS
				std::cout << "[RSA] Got to the part that Liam is confused about\n";
				std::cout << "[RSA] Got from next, sending backwards\n";
				Helper::sendVector(circuitData[rr.circuit_id].first, requestInfo.buffer);
			}
			else throw std::runtime_error("There is an error with the key exchanges, got from unknown socket");
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