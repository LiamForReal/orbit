#pragma once
#include "IRequestHandler.h"

class NodeOpeningHandler : virtual public IRequestHandler
{
	public:
		NodeOpeningHandler(DockerManager& dockerManager, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, std::map<unsigned int, SOCKET>& clients);
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		RequestResult handleRequest(const RequestInfo& requestInfo) override;

	private:
		static unsigned int circuit_id;
		RequestResult rr;
		DockerManager& dm;
		std::map<unsigned int, SOCKET>& _clients;
		std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList;
};