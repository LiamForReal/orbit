#include "RsaKeyExchangeRequestHandler.h"

RsaKeyExchangeRequestHandler::RsaKeyExchangeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, std::map<unsigned int, AES>& aesKeys)
	:_circuitData(circuitData), _socket(s), _rsaKeys(rsaKeys), _aesKeys(aesKeys)
{
	this->rr = RequestResult();
}

bool RsaKeyExchangeRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) 
{
	return requestInfo.id == RSA_KEY_EXCHANGE_RC;
}

RequestResult RsaKeyExchangeRequestHandler::handleRequest(RequestInfo& requestInfo)
{
	RequestInfo ri;
	RsaKeyExchangeResponse rkeResponse;
	unsigned int status = RSA_KEY_EXCHANGE_STATUS;
	rr.buffer.clear();
	try
	{
		unsigned int circuit_id = requestInfo.circuit_id;
		if (_circuitData.find(circuit_id) == _circuitData.end())
		{
			_circuitData[circuit_id].first = _socket;
			std::cout << "[RSA] client socket allocated!\n";
		}

		if (_circuitData[circuit_id].first == _socket)
		{
			if (_circuitData[circuit_id].second != INVALID_SOCKET && _circuitData[circuit_id].second != NULL)
			{
				// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NEXT
				// SEND TO NEXT AND WAIT FOR RESPONSE AND THEN SEND BACKWARDS
				std::cout << "[RSA] Got from prev, there is next, sending and listening forward\n";
				rr.buffer = Helper::buildRR(requestInfo);
				Helper::sendVector(_circuitData[circuit_id].second, rr.buffer);
				ri = Helper::waitForResponse(_circuitData[circuit_id].second);//sends rr but I put that on ri
				if (_aesKeys.find(circuit_id) != _aesKeys.end())
				{
					ri.buffer = _aesKeys[circuit_id].encrypt(ri.buffer);
					std::cout << "[RSA] msg encript by this aes layer\n";
				}
				std::cout << "[RSA] sending backwards\n";
				rr.buffer = Helper::buildRR(ri);
				Helper::sendVector(_circuitData[circuit_id].first, rr.buffer);
			}
			else
			{
				RsaKeyExchangeRequest rkeRequest = DeserializerRequests::deserializeRsaKeyExchangeRequest(requestInfo);
				// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NO NEXT
				// SAVE RSA AND SEND BACKWARDS
				std::cout << "[RSA] Got from prev, there is no next, saving and sending backwards\n";
				// TODO: HERE CREATE THE RSA OF THE NODE
				RSA rsa = RSA();
				rsa.pregenerateKeys();
				std::cout << "[RSA] created for circuit " << circuit_id << std::endl;
				_rsaKeys[circuit_id] = std::pair<RSA, std::pair<uint2048_t, uint2048_t>>(
					rsa, std::pair<uint2048_t, uint2048_t>(
						rkeRequest.public_key, rkeRequest.product
					));
				rkeResponse.public_key = rsa.getPublicKey();
				rkeResponse.product = rsa.getProduct();
				std::vector<unsigned char> data = SerializerResponses::serializeResponse(rkeResponse);
				rr.buffer = Helper::buildRR(data, status, data.size(), circuit_id);
				Helper::sendVector(_circuitData[circuit_id].first, rr.buffer);
			}
		}
		else throw std::runtime_error("[RSA] There is an error with the key exchanges, got from unknown socket");
	}
	catch (std::runtime_error& e)
	{
		status = RSA_KEY_EXCHANGE_ERROR;
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "[RSA] unecpected error!!!";
		throw;
	}
	return rr;
}