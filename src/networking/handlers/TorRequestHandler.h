#pragma once
#include "NodeOpeningHandler.h"
#include "DeleteCircuitRequestHandler.h"
#include "../../utils/AES.h"

class TorRequestHandler
{
	public: 
		~TorRequestHandler();
		TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList, std::map<unsigned int, SOCKET>& clients, AES& aes);
		RequestResult directRequest(RequestInfo& requestInfo);

	private:
		NodeOpeningHandler* noh;//more...
		DockerManager& dm;

		AES _aes;
		std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList;
		std::map<unsigned int, SOCKET>& _clients;
};