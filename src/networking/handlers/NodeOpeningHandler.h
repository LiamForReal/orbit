#pragma once
#include "IRequestHandler.h"

class NodeOpeningHandler : virtual public IRequestHandler
{
	public:
		NodeOpeningHandler(DockerManager& dockerManager);
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		RequestResult handleRequest(const RequestInfo& requestInfo) override;
	private:
		static unsigned int circuit_id;
		RequestResult rr;
		DockerManager& dm;
};