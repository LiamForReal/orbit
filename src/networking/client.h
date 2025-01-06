#pragma comment (lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include "WSAInitializer.h"
#include <atomic>
#include <condition_variable>
#include <stdexcept>
#include <mutex>
#include "../utils/Requestes.hpp"
#include "../utils/Responses.hpp"
#include "../utils/DeserializerResponses.h"
#include "../utils/SerializerRequests.h"
#include "../utils/Helper.h"
#include "../utils/RSA.h"

#define COMMUNICATE_NODE_PORT 9050
#define COMMUNICATE_SERVER_PORT 9787
#define MAX_NODES_TO_OPEN 5
#define MIN_NODES_TO_OPEN 1

class Client
{
public:
	Client();
	~Client();
	void connectToServer(std::string serverIP, int port);
	void startConversation(const bool& openNodes = true);
	void nodeOpening();
	bool domainValidationCheck(std::string domain);
	void listenToServerInfo();

	bool getPassedListenWait() const;
	bool getPassedPathGetWait() const;
	bool getRestartConversation() const;

	void setPassedPathGetWait(const bool& passedPathGetWait);
	void setRestartConversation(const bool& restartConversation);

	void closeSocketWithFirstNode();
	
private:
	
	std::string generateHttpGetRequest(const std::string& domain);

	bool _passedListenWait;
	bool _passedPathGetWait;
	bool _restartConversation;

	SOCKET _clientSocketWithFirstNode;
	SOCKET _clientSocketWithDS;

	RSA rsa;
	uint2048_t rsaServerPubkey;
	uint2048_t rsaServerProduct;
	std::vector<std::pair<uint2048_t, uint2048_t>> rsaCircuitData;
};