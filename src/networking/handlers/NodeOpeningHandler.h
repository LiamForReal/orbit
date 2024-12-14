#pragma once
#include "IRequestHandler.h"

class NodeOpeningHandler : virtual public IRequestHandler
{
	public:
		NodeOpeningHandler(DockerManager& dockerManager, std::map<unsigned int, std::list<std::pair<std::string, std::string>>>& controlList);
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		RequestResult handleRequest(const RequestInfo& requestInfo) override;

		static unsigned int getCircuitID();

	private:
		static unsigned int circuit_id;
		RequestResult rr;
		DockerManager& dm;

		std::map<unsigned int, std::list<std::pair<std::string, std::string>>>& _controlList;
};