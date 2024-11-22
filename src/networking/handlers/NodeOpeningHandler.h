#pragma once
#include "IRequestHandler.h"

class NodeOpeningHandler : virtual public IRequestHandler
{
	public:
		NodeOpeningHandler();
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		RequestResult handleRequest(const RequestInfo& requestInfo, DockerManager dm) override;

	private:
		RequestResult rr;
};