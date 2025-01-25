#include "client.h"
using std::vector;

std::mutex mtx;

Client::Client()
{
	// we connect to server that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocketWithDS = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_clientSocketWithFirstNode = INVALID_SOCKET;

	if (_clientSocketWithDS == INVALID_SOCKET)
		throw std::runtime_error("server run error socket");

	this->_passedListenWait = false;
	this->_passedPathGetWait = false;
	this->_restartConversation = false;
}

Client::~Client()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		_rsaCircuitData.clear();
		closesocket(_clientSocketWithFirstNode);
		closesocket(_clientSocketWithDS);
	}
	catch (...) {}
}

void Client::connectToServer(std::string serverIP, int port)
{
	RequestResult rr;
	struct sockaddr_in sa = { 0 };
	RequestInfo ri;
	sa.sin_port = htons(port); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());    // the IP of the server

	// the process will not continue until the server accepts the client
	if (connect(_clientSocketWithDS, (struct sockaddr*)&sa, sizeof(sa)) == INVALID_SOCKET)
		throw std::runtime_error("Cant connect to server");
	//CREATE SELF RSA PAIR OF KEYS START
	_rsa.pregenerateKeys(); 
	std::cout << "Client finished pregenerating RSA keys...\n";
	//CREATE SELF RSA PAIR OF KEYS END
	
	//RSA KEY EXCHANGE WITH DS START
	RsaKeyExchangeRequest rkeRequest;
	rkeRequest.public_key = _rsa.getPublicKey();
	rkeRequest.product = _rsa.getProduct();
	rr.buffer = Helper::buildRR(SerializerRequests::serializeRequest(rkeRequest), RSA_KEY_EXCHANGE_RC);
	try
	{
		Helper::sendVector(_clientSocketWithDS, rr.buffer);
		ri = Helper::waitForResponse(_clientSocketWithDS);
	}
	catch (...)
	{
		std::cerr << "Problem with RSA\n";
	}
	if (RSA_KEY_EXCHANGE_STATUS == ri.id)
	{
		RsaKeyExchangeResponse rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri.buffer);
		_serverRSA.first = rkeResponse.public_key;
		_serverRSA.second = rkeResponse.product;
		std::cout << "Got server's RSA public key: " << _serverRSA.first << std::endl;
	}
	else
	{
		std::cerr << "Could not exchange RSA keys with server!\n";
		exit(1);
	}
	//RSA KEY EXCHANGE WITH DS END

	//SEND REQUEST AND BUILD  ECDHE INFO START
	EcdheKeyExchangeRequest ekeRequest;
	EcdheKeyExchangeResponse ekeResponse;
	try
	{
		std::cout << "ecdhe first msg is now generating\n";
		std::pair<uint256_t, uint256_t> ecdheInfo = _ecdhe.createInfo(); //(g, p) 
		ekeRequest.b = ecdheInfo.first;
		ekeRequest.m = ecdheInfo.second;
		ekeRequest.calculationResult = _ecdhe.createDefiKey();
		rr.buffer = Helper::buildRR(_rsa.Encrypt(SerializerRequests::serializeRequest(ekeRequest), _serverRSA.first, _serverRSA.second), ECDHE_KEY_EXCHANGE_RC);
		Helper::sendVector(_clientSocketWithDS, rr.buffer);
		//SEND REQUEST AND BUILD ECDHE INFO END

		//BUILD AES KEY START
		ri = Helper::waitForResponse_RSA(_clientSocketWithDS, _rsa);

		if (ECDHE_KEY_EXCHANGE_STATUS != ri.id)
		{
			throw std::runtime_error("Did not get ECDHE key exchange request!");
		}
		std::cout << "ecdhe msg recved\n";
		
		ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri.buffer);
		_ecdhe.setG(ekeResponse.calculationResult);
		std::cout << "generate aes key!!!\n";
		_serverAes = _ecdhe.createDefiKey();
		std::cout << "shered sicret is: " << _serverAes << "\n";
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
	//BUILD AES KEY END
}

void Client::nodeOpening()
{
	NodeOpenRequest nor;
	RequestResult rr;
	do
	{
		std::cout << "enter amount of nodes to open (between " + std::to_string(MIN_NODES_TO_OPEN) + " - " + std::to_string(MAX_NODES_TO_OPEN) + "): ";
		std::cin >> nor.amount_to_open;
		std::cout << "enter amount of nodes to use: ";
		std::cin >> nor.amount_to_use;
	} while (nor.amount_to_open > MAX_NODES_TO_OPEN || nor.amount_to_open < MIN_NODES_TO_OPEN || nor.amount_to_use < MIN_NODES_TO_OPEN);
	rr.buffer = Helper::buildRR(SerializerRequests::serializeRequest(nor), NODE_OPEN_RC);
	Helper::sendVector(_clientSocketWithDS, rr.buffer);
	std::cout << "Message send to server..." << std::endl;
}

std::string Client::generateHttpGetRequest(const std::string& domain) 
{
	std::ostringstream request;
	request << domain;
	return request.str();
}

bool Client::domainValidationCheck(std::string domain)
{
	//acomplish it
	return true;
}

void Client::listenToServerInfo()
{
	try
	{
		RequestInfo ri;
		while (true)
		{
			this->_passedListenWait = false;
			ri = Helper::waitForResponse(this->_clientSocketWithDS);
			this->_passedListenWait = true;
			if (ri.buffer.empty())
				continue;
			else if (ri.id == DELETE_CIRCUIT_RC)
			{
				//TOCHANGE BY MAIN 
				std::cout << "got delete!!!\n";
				this->_restartConversation = true;
				throw std::runtime_error("the circuit is corrupted!");
			}
			std::cout << "server sends " << ri.id << " request tipe\n";
		}
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
		if (e.what() == "the circuit is corrupted!")
		{
			throw std::runtime_error("adjust new circuit");
		}
	}
	catch (...)
	{
		std::cout << "an unaccespted error\n";
	}
}

bool Client::getPassedListenWait() const
{
	return this->_passedListenWait;
}

bool Client::getPassedPathGetWait() const
{
	return this->_passedPathGetWait;
}

bool Client::getRestartConversation() const
{
	return this->_restartConversation;
}

void Client::setPassedPathGetWait(const bool& passedPathGetWait)
{
	this->_passedPathGetWait = passedPathGetWait;
}

void Client::setRestartConversation(const bool& restartConversation)
{
	this->_restartConversation = restartConversation;
}

void Client::closeSocketWithFirstNode()
{
	closesocket(this->_clientSocketWithFirstNode);
	this->_clientSocketWithFirstNode = NULL;
}

void Client::startConversation(const bool& openNodes)
{
	char buffer[100];
	std::string domain;
	RequestInfo ri;
	RequestResult rr;
	//NODE OPPENING START
	if (openNodes)
	{
		nodeOpening();
	}
	ri = Helper::waitForResponse(this->_clientSocketWithDS); //problem
	if (openNodes)
	{
		this->_passedPathGetWait = true;
	}
	unsigned int circuit_id = ri.circuit_id;
	CircuitConfirmationResponse ccr = DeserializerResponses::deserializeCircuitConfirmationResponse(ri.buffer);
	_rsaCircuitData.reserve(ccr.nodesPath.size());

	for (auto it = ccr.nodesPath.begin(); it != ccr.nodesPath.end(); it++)
	{
		std::cout << "Node: " << it->first << " " << it->second << std::endl;
	}
	
	//NODE OPPENING END

	//CONNECTING TO FIRST NODE START
	_clientSocketWithFirstNode = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (_clientSocketWithFirstNode == INVALID_SOCKET)
		throw std::runtime_error("Client run error socket");

	int optval = 1;
	if (setsockopt(_clientSocketWithFirstNode, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) != 0) {
		perror("Failed to set socket option SO_REUSEADDR");
	}

	std::cout << "Connecting to " << ccr.nodesPath.begin()->first << " " << ccr.nodesPath.begin()->second << std::endl;

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(stoi(ccr.nodesPath.begin()->second));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(ccr.nodesPath.begin()->first.c_str());
	if (connect(_clientSocketWithFirstNode, (struct sockaddr*)&sa, sizeof(sa)) == INVALID_SOCKET)
	{
		int errCode = WSAGetLastError();  // Get the error code from Winsock
		std::cerr << "Connect failed with error code: " << errCode << std::endl;
		throw std::runtime_error("Could not open socket with first node");
	}
	else std::cout << "connected successfully to the first node\n";

	//CONNECTING TO FIRST NODE END

	//SEND RSA KEY EXCHANGE TO FIRST NODE START
	RequestResult rrRSA;
	LinkRequest linkRequest;
	RsaKeyExchangeRequest rkeRequest;
	RsaKeyExchangeResponse rkeResponse;
	rkeRequest.public_key = _rsa.getPublicKey();
	rkeRequest.product = _rsa.getProduct();
	rrRSA.buffer = Helper::buildRR(SerializerRequests::serializeRequest(rkeRequest), RSA_KEY_EXCHANGE_RC ,circuit_id);
	//to Change the make msges logic
	Helper::sendVector(_clientSocketWithFirstNode, rrRSA.buffer);
	std::cout << "sent RSA msg\n";

	ri = Helper::waitForResponse(_clientSocketWithFirstNode);
	rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri.buffer);

	if (Status::RSA_KEY_EXCHANGE_STATUS == ri.id)
	{
		std::cout << "Got FIRST NODE public_key: " << rkeResponse.public_key << std::endl;
		_rsaCircuitData.emplace_back(rkeResponse.public_key, rkeResponse.product);
		std::cout << "Saved FIRST NODE pubkey\n";
	}
	else
	{
		throw std::runtime_error("Could not exchange RSA keys, thus could not build circuit.");
	}
	ri.buffer.clear();
	//SEND RSA KEY EXCHANGE TO FIRST NODE END
	
	//SEND ECDHE KEY EXCHANGE TO FIRST NODE START
	EcdheKeyExchangeRequest ekeRequest;
	EcdheKeyExchangeResponse ekeResponse;
	auto nodePlaceIt = _rsaCircuitData.begin();
	try
	{
		std::cout << "ecdhe with first node msg is now generating\n";
		std::pair<uint256_t, uint256_t> ecdheInfo = _ecdhe.createInfo(); //(g, p) 
		ekeRequest.b = ecdheInfo.first;
		ekeRequest.m = ecdheInfo.second;
		ekeRequest.calculationResult = _ecdhe.createDefiKey();
		rr.buffer = Helper::buildRR(_rsa.Encrypt(SerializerRequests::serializeRequest(ekeRequest), nodePlaceIt->first, nodePlaceIt->second), ECDHE_KEY_EXCHANGE_RC, circuit_id);
		std::cout << "ecdhe with first node msg is now sending\n";
		Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);

		ri = Helper::waitForResponse_RSA(_clientSocketWithFirstNode, _rsa);

		if (ECDHE_KEY_EXCHANGE_STATUS != ri.id)
		{
			throw std::runtime_error("Did not get ECDHE key exchange response status!");
		}
		std::cout << "ecdhe msg recved\n";

		ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri.buffer);
		_ecdhe.setG(ekeResponse.calculationResult);
		std::cout << "generate aes key!!!\n";
		_aesCircuitData.emplace_back(_ecdhe.createDefiKey());
		std::cout << "shered sicret with first node is: " << _aesCircuitData[_aesCircuitData.size() - 1] << "\n";
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
	//SEND ECDHE KEY EXCHANGE TO FIRST NODE END
	
    //SENDING LINK AND RSA KEY EXCHANGE AND ECDHE KEY EXCHANGE TO ALL THE REST NODES START
	if (ccr.nodesPath.size() > 1)
	{
		for (auto it = ccr.nodesPath.begin() + 1; it != ccr.nodesPath.end(); it++)
		{
			linkRequest.nextNode = std::pair<std::string, unsigned int>(it->first, stoi(it->second));
			rr.buffer = Helper::buildRR(SerializerRequests::serializeRequest(linkRequest), LINK_RC ,circuit_id);
			Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
			std::cout << "sent link msg\n";

			
			ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			if (Errors::LINK_ERROR == ri.id)
			{
				throw std::runtime_error("Could not build circuit.");
			}
			
			Helper::sendVector(_clientSocketWithFirstNode, rrRSA.buffer);
			std::cout << "sent RSA msg\n";
			ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri.buffer);

			if (Status::RSA_KEY_EXCHANGE_STATUS == ri.id)
			{
				std::cout << "Got NODE public_key: " << rkeResponse.public_key << std::endl;
				_rsaCircuitData.emplace_back(rkeResponse.public_key, rkeResponse.product);
				nodePlaceIt++;
				std::cout << "Saved NODE pubkey\n";
			}
			else
			{
				throw std::runtime_error("Could not exchange RSA keys, thus could not build circuit.");
			}

			try
			{
				std::cout << "ecdhe with first node msg is now generating\n";
				std::pair<uint256_t, uint256_t> ecdheInfo = _ecdhe.createInfo(); //(g, p) 
				ekeRequest.b = ecdheInfo.first;
				ekeRequest.m = ecdheInfo.second;
				ekeRequest.calculationResult = _ecdhe.createDefiKey();
				rr.buffer = Helper::buildRR(_rsa.Encrypt(SerializerRequests::serializeRequest(ekeRequest), nodePlaceIt->first, nodePlaceIt->second), ECDHE_KEY_EXCHANGE_RC, circuit_id);
				Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
				
				ri = Helper::waitForResponse_RSA(_clientSocketWithFirstNode, _rsa);

				if (ECDHE_KEY_EXCHANGE_STATUS != ri.id)
				{
					throw std::runtime_error("Did not get ECDHE key exchange response status!");
				}
				std::cout << "ecdhe msg recved\n";

				ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri.buffer);
				_ecdhe.setG(ekeResponse.calculationResult);
				std::cout << "generate aes key!!!\n";
				_aesCircuitData.emplace_back(_ecdhe.createDefiKey());
				std::cout << "shered sicret with first node is: " << _aesCircuitData[_aesCircuitData.size() - 1] << "\n";
			}
			catch (std::runtime_error e)
			{
				std::cout << e.what() << std::endl;
			}

		}
	}
	//SENDING LINK AND RSA KEY EXCHANGE AND ECDHE KEY EXCHANGE TO ALL THE REST NODES END

	//SENDING HTTP GET START
	HttpGetRequest httpGetRequest;

	std::cout << "Enter domain: ";
	std::cin >> domain;
	if (!domainValidationCheck(domain))
		throw std::runtime_error("domain is illegal");
	httpGetRequest.domain = domain;
	rr.buffer = Helper::buildRR(SerializerRequests::serializeRequest(httpGetRequest),HTTP_MSG_RC,circuit_id);

	Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
	std::cout << "sends httpGet Request:\n";
	ri = Helper::waitForResponse(_clientSocketWithFirstNode);

	HttpGetResponse httpGetResponse;
	httpGetResponse = DeserializerResponses::deserializeHttpGetResponse(ri.buffer);

	if (Errors::HTTP_MSG_ERROR == ri.id)
	{
		std::cerr << "Could not get HTML of " << domain << std::endl;
	}
	else
	{
		std::cout << "HTML of " << domain << ": " << std::endl;
		std::cout << httpGetResponse.content << std::endl;
	}
	//SENDING HTTP GET END
}

int main()
{
	try
	{
		WSAInitializer wsa = WSAInitializer();
		Client client = Client();
		client.connectToServer("127.0.0.1", COMMUNICATE_SERVER_PORT);
		//client.startConversation();

		std::thread startConversationThread(&Client::startConversation, std::ref(client), true);
		startConversationThread.detach();

		while (true)
		{
			if (client.getPassedPathGetWait())
			{
				//std::thread listenToServerInfoThread(&Client::listenToServerInfo, std::ref(client));
				//listenToServerInfoThread.detach();
				client.setPassedPathGetWait(false);
			}
			if (client.getRestartConversation())
			{
				startConversationThread.~thread();
				client.setRestartConversation(false);
				client.closeSocketWithFirstNode();
				startConversationThread = std::thread(&Client::startConversation, std::ref(client), false);
				startConversationThread.detach();
			}
		}

		//thread listen to ds -> this thread start only after startConversation is got the input from ds 
		// boolian property after recving (wait for function...)
		// 
		//when the thread is getting DELETE CIRCUIT REQUEST it suppose to stop immidiatly the operating of startConvarsation and to reRunIt
		//to put start convarsation on thread. -> add another boolian -> look at the picture 
		// 
		//it should happend always eaven after it happends one time it should continue to check
	}
	catch (const std::runtime_error e)
	{
		std::cerr << e.what() << '\n';
	}
	catch (...)
	{
		std::cout << "an unaccespted error\n";
	}
	system("pause");
	return 0;
}
