#pragma once
#include "NodeOpeningHandler.h"
#include "DeleteCircuitRequestHandler.h"
#include "../../utils/AES.h"
#include "CloseConnectionHandler.h"
class TorRequestHandler
{
	public: 
		~TorRequestHandler();
		TorRequestHandler(DockerManager& newDm, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList, std::map<unsigned int, SOCKET>& clients, AES& aes);
		RequestResult directRequest(RequestInfo& requestInfo);

	private:
		NodeOpeningHandler* noh;//more...
		CloseConnectionHandler* cch;
		DockerManager& dm;


		AES _aes;
		std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& _controlList;
		std::map<unsigned int, SOCKET>& _clients;
};