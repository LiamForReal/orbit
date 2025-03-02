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
#include "../utils/ECDHE.h"
#include "../utils/AES.h"

typedef struct RequestResult // Changed from typedef to struct definition
{
	std::vector<unsigned char> buffer;
} RequestResult;

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
	void HandleTorClient(const bool& regular = true);
	void nodeOpening(const bool& regular);
	bool domainValidationCheck(std::string domain);

	void dataLayersEncription(std::vector<unsigned char>& data);
	void dataLayersDecription(std::vector<unsigned char>& data);
	void closeSocketWithFirstNode();
	
private:
	
	std::string generateHttpGetRequest(const std::string& domain);

	unsigned int circuit_id;
	SOCKET _clientSocketWithDS;
	SOCKET _clientSocketWithFirstNode;

	RSA _rsa; //self
	std::pair<uint2048_t, uint2048_t> _serverRSA; //server's
	std::vector<std::pair<uint2048_t, uint2048_t>> _rsaCircuitData; //circuit's

	ECDHE _ecdhe; //self 

	AES _aes;
	std::vector<AES> _aesCircuitData;
};