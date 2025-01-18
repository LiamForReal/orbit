#include "DeleteCircuitRequestHandler.h"

DeleteCircuitRequestHandler::DeleteCircuitRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData, SOCKET& clientSock)
	: cd(circuitsData), _socket(clientSock)
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

RequestResult DeleteCircuitRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	this->rr.buffer.clear();
	DeleteCircuitResponse dcre;
	unsigned int circuit_id = requestInfo.circuit_id;
	try
	{
		closeSocket(this->cd[requestInfo.circuit_id].first);
		closeSocket(this->cd[requestInfo.circuit_id].second);
		this->cd.erase(requestInfo.circuit_id);
		dcre.status = DELETE_CIRCUIT_STATUS;
	}
	catch (std::runtime_error& e)
	{
		dcre.status = DELETE_CIRCUIT_ERROR;
		std::cout << e.what() << std::endl;
	}
	rr.buffer = Helper::buildRR(SerializerResponses::serializeResponse(dcre), dcre.status, circuit_id);
	return rr;
}