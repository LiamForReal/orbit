#include "DeleteCircuitRequestHandler.h"

DeleteCircuitRequestHandler::DeleteCircuitRequestHandler(DockerManager& dockerManager, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, unsigned int& crId)
	: dm(dockerManager), _controlList(controlList), circuit_id(crId)
{
	this->rr = RequestResult();
}

bool DeleteCircuitRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == DELETE_CIRCUIT_RC;
}

RequestResult DeleteCircuitRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	return rr;
	try
	{

	}
	catch (std::runtime_error& e)
	{

	}
}