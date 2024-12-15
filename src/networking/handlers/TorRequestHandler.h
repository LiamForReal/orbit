#pragma once
#include "NodeOpeningHandler.h"
#include "DeleteCircuitRequestHandler.h"

class TorRequestHandler
{
	public: 
		~TorRequestHandler();
		TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList);
		RequestResult directRequest(const RequestInfo& requestInfo);
		unsigned int circuitId;

	private:
		NodeOpeningHandler* noh;//more...
		DeleteCircuitRequestHandler* dcrh;
		DockerManager& dm;
		std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList;
};