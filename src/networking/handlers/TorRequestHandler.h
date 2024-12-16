#pragma once
#include "NodeOpeningHandler.h"
#include "DeleteCircuitRequestHandler.h"

class TorRequestHandler
{
	public: 
		~TorRequestHandler();
		TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList);
		RequestResult directRequest(const RequestInfo& requestInfo);

	private:
		NodeOpeningHandler* noh;//more...
		DockerManager& dm;
		std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList;
};