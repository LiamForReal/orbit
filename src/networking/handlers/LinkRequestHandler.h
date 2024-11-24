#pragma once
#include "IRequestHandler.h"

class LinkRequestHandler : virtual public IRequestHandler
{
public:
	LinkRequestHandler();
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:
	RequestResult rr;
};