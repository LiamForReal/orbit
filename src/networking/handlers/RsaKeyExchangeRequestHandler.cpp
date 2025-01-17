#include "RsaKeyExchangeRequestHandler.h"

RsaKeyExchangeRequestHandler::RsaKeyExchangeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys)
	:_circuitData(circuitData), _socket(s), _rsaKeys(rsaKeys)
{
	this->rr = RequestResult();
}

bool RsaKeyExchangeRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) 
{
	return requestInfo.id == RSA_KEY_EXCHANGE_RC;
}

RequestResult RsaKeyExchangeRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	RequestInfo ri;
	RsaKeyExchangeRequest rkeRequest;
	RsaKeyExchangeResponse rkeResponse;
	rr.buffer.clear();
	try
	{
		rr.circuit_id = requestInfo.circuit_id;
		rkeRequest = DeserializerRequests::deserializeRsaKeyExchangeRequest(requestInfo.buffer);

		rkeResponse.status = RSA_KEY_EXCHANGE_STATUS;

		if (_circuitData.find(rr.circuit_id) == _circuitData.end())
		{
			_circuitData[rr.circuit_id].first = _socket;
		}

		if (_circuitData[rr.circuit_id].first == _socket)
		{
			if (_circuitData[rr.circuit_id].second != INVALID_SOCKET && _circuitData[rr.circuit_id].second != NULL)
			{
				// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NEXT
				// SEND TO NEXT AND WAIT FOR RESPONSE AND THEN SEND BACKWARDS
				std::cout << "[RSA] Got from prev, there is next, sending and listening forward\n";
				Helper::sendVector(_circuitData[rr.circuit_id].second, requestInfo.buffer);
				ri = Helper::waitForResponse(_circuitData[rr.circuit_id].second);//sends rr but I put that on ri
				std::cout << "[RSA] sending backwards\n";
				rr.buffer = ri.buffer;
				Helper::sendVector(_circuitData[rr.circuit_id].first, rr.buffer);
			}
			else
			{
				// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NO NEXT
				// SAVE RSA AND SEND BACKWARDS
				std::cout << "[RSA] Got from prev, there is no next, saving and sending backwards\n";
				// TODO: HERE CREATE THE RSA OF THE NODE
				RSA rsa;
				rsa.pregenerateKeys();
				std::cout << "RSA created for circuit " << rr.circuit_id << std::endl;
				_rsaKeys[rr.circuit_id] = std::pair<RSA, std::pair<uint2048_t, uint2048_t>>(rsa, std::pair<uint2048_t, uint2048_t>(rkeRequest.public_key, rkeRequest.product));
				rkeResponse.public_key = _rsaKeys[rr.circuit_id].first.getPublicKey();
				rkeResponse.product = _rsaKeys[rr.circuit_id].first.getProduct();
				rr.buffer.emplace_back(unsigned char(rr.circuit_id));
				vector<unsigned char> tmp = SerializerResponses::serializeResponse(rkeResponse);
				rr.buffer.insert(rr.buffer.end(), tmp.begin(), tmp.end());
				Helper::sendVector(_circuitData[rr.circuit_id].first, rr.buffer);
			}
		}
		else throw std::runtime_error("There is an error with the key exchanges, got from unknown socket");
	}
	catch (std::runtime_error& e)
	{
		rkeResponse.status = RSA_KEY_EXCHANGE_ERROR;
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unecpected error!!!";
	}
	return rr;
}