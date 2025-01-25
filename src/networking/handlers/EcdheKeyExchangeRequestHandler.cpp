#include "EcdheKeyExchangeRequestHandler.h"

EcdheKeyExchangeRequestHandler::EcdheKeyExchangeRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitData, SOCKET& s, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, std::map<unsigned int, uint256_t>& aesKeys)
	: _circuitData(circuitData), _socket(s), _rsaKeys(rsaKeys), _aesKeys(aesKeys)
{
	this->_ecdheInfo = std::map<unsigned int, ECDHE>();
	this->rr = RequestResult();
}

bool EcdheKeyExchangeRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == ECDHE_KEY_EXCHANGE_RC;
}

RequestResult EcdheKeyExchangeRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	RequestInfo ri;
	EcdheKeyExchangeRequest ekeRequest;
	EcdheKeyExchangeResponse ekeResponse;
	unsigned int status = ECDHE_KEY_EXCHANGE_STATUS;
	rr.buffer.clear();
	try
	{
		ekeRequest = DeserializerRequests::deserializeEcdheKeyExchangeRequest(requestInfo.buffer);
		unsigned int circuit_id = requestInfo.circuit_id;
		std::cout << "[ECDHE] circuit id: " << circuit_id << "\n";
		if (_circuitData[circuit_id].first == _socket)
		{
			if (_circuitData[circuit_id].second != INVALID_SOCKET && _circuitData[circuit_id].second != NULL)
			{
				// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NEXT
				// SEND TO NEXT AND WAIT FOR RESPONSE AND THEN SEND BACKWARDS
				std::cout << "[ECDHE] Got from prev, there is next, sending and listening forward\n";
				rr.buffer = Helper::buildRR(requestInfo);
				Helper::sendVector(_circuitData[circuit_id].second, rr.buffer);
				ri = Helper::waitForResponse(_circuitData[circuit_id].second);//sends rr but I put that on ri
				std::cout << "[ECDHE] sending backwards\n";
				rr.buffer = Helper::buildRR(ri);
				Helper::sendVector(_circuitData[circuit_id].first, rr.buffer);
			}
			else
			{
				// RSA HANDLING IF GOT MSG FROM PREV AND THERE IS NO NEXT
				// SAVE RSA AND SEND BACKWARDS
				std::cout << "[ECDHE] Got from prev, there is no next, saving and sending backwards\n";
				//create defi HelmanKey and save it in ecdhe info
				_ecdheInfo[circuit_id].setG(ekeRequest.b);
				_ecdheInfo[circuit_id].setP(ekeRequest.m);
				_ecdheInfo[circuit_id].createTmpKey();
				ekeResponse.calculationResult = _ecdheInfo[circuit_id].createDefiKey();
				std::cout << "[ECDHE] created for circuit " << circuit_id << std::endl;

				rr.buffer = Helper::buildRR(_rsaKeys[circuit_id].first.Encrypt(SerializerResponses::serializeResponse(ekeResponse), _rsaKeys[circuit_id].second.first, _rsaKeys[circuit_id].second.second)
					, status, circuit_id);
				std::cout << "[ECDHE] send to client rsa encripted msg\n";

				Helper::sendVector(_circuitData[circuit_id].first, rr.buffer);

				_ecdheInfo[circuit_id].setG(ekeRequest.calculationResult);
				std::cout << "[ECDHE] generate aes key!!!\n";
				_aesKeys[circuit_id] = _ecdheInfo[circuit_id].createDefiKey();
				std::cout << "[ECDHE] shered sicret is: " << _aesKeys[circuit_id] << "\n";
				//nothing to send!!! 
				//decript RSA + found the sherd seacret + and put is in AES 
			}
		}
		else throw std::runtime_error("There is an error with the key exchanges, got from unknown socket");
	}
	catch (std::runtime_error& e)
	{
		status = RSA_KEY_EXCHANGE_ERROR;
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unecpected error!!!";
	}
	return rr;
}