#include "DeleteCircuitRequestHandler.h"

DeleteCircuitRequestHandler::DeleteCircuitRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData, std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>>& rsaKeys, std::map<unsigned int, AES>& aesKeys, SOCKET& socket)
	: cd(circuitsData), _rsaKeys(rsaKeys), _aesKeys(aesKeys), _socket(socket)
{
	this->rr = RequestResult();
}

void DeleteCircuitRequestHandler::closeSocket(SOCKET socketToClose)
{
	if (closesocket(socketToClose) == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		std::cout << "Failed to close socket. Error code: " << errorCode << std::endl;
		throw std::runtime_error("Failed to close socket");
	}
	else
	{
		std::cout << "Socket successfully closed.\n";
	}
}
bool DeleteCircuitRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == DELETE_CIRCUIT_RC;
}

RequestResult DeleteCircuitRequestHandler::handleRequest(RequestInfo& requestInfo)
{
	rr.buffer.clear();
	unsigned int circuit_id = requestInfo.circuit_id,status = DELETE_CIRCUIT_STATUS;
	try
	{
		closeSocket(cd[requestInfo.circuit_id].first);
		cd[requestInfo.circuit_id].first = INVALID_SOCKET;
		closeSocket(cd[requestInfo.circuit_id].second);
		cd[requestInfo.circuit_id].second = INVALID_SOCKET;
		_socket = INVALID_SOCKET;
		cd.erase(requestInfo.circuit_id);
		_rsaKeys.erase(requestInfo.circuit_id);
		_aesKeys.erase(requestInfo.circuit_id);
		std::cout << "[DELETE CIRCUIT] circuit was deleted successfully!\n";
	}
	catch (std::runtime_error& e)
	{
		status = DELETE_CIRCUIT_ERROR;
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		status = DELETE_CIRCUIT_ERROR;
		std::cout << "[DELETE CIRCUIT] an anexpecte error accured while deleting";
	}
	rr.buffer = Helper::buildRR(status, circuit_id);
	return rr;
}