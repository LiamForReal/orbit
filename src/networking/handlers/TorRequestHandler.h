#pragma once
#include "NodeOpeningHandler.h"

class TorRequestHandler
{
public: 
	~TorRequestHandler();
	TorRequestHandler(DockerManager& newDm);
	RequestResult directRequest(const RequestInfo& requestInfo);
private:
	NodeOpeningHandler* noh;//more...
	DockerManager& dm;
};