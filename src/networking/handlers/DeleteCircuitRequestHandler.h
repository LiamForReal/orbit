#pragma once 
#include "IRequestHandler.h"

class DeleteCircuitRequestHandler : virtual public IRequestHandler
{
public:
	DeleteCircuitRequestHandler(DockerManager& dockerManager, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, unsigned int& crId);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:		
	RequestResult rr;
	DockerManager& dm;
	unsigned int& circuit_id;
	std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList;
};