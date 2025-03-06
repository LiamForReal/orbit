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
	std::vector<unsigned char> data;
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
	data = SerializerRequests::serializeRequest(rkeRequest);
	rr.buffer = Helper::buildRR(data, RSA_KEY_EXCHANGE_RC, data.size());
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
		RsaKeyExchangeResponse rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri);
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
		std::vector<unsigned char> data = _rsa.Encrypt(SerializerRequests::serializeRequest(ekeRequest), _serverRSA.first, _serverRSA.second);
		rr.buffer = Helper::buildRR(data, ECDHE_KEY_EXCHANGE_RC, data.size());
		Helper::sendVector(_clientSocketWithDS, rr.buffer);
		//SEND REQUEST AND BUILD ECDHE INFO END

		//BUILD AES KEY START
		ri = Helper::waitForResponse_RSA(_clientSocketWithDS, _rsa);

		if (ECDHE_KEY_EXCHANGE_STATUS != ri.id)
		{
			throw std::runtime_error("Did not get ECDHE key exchange request!");
		}
		std::cout << "ecdhe msg recved\n";
		
		ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri);
		_ecdhe.setG(ekeResponse.calculationResult);
		std::cout << "generate aes key!!!\n";
		uint256_t sheredSicret = _ecdhe.createDefiKey();
		_aes.generateRoundKeys(sheredSicret);
		std::cout << "shered sicret is: " << sheredSicret << "\n";
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
	std::vector<unsigned char> data;
	std::cout << "node openning begins:\n";
	do
	{
		std::cout << "enter amount of nodes to open (between " + std::to_string(MIN_NODES_TO_OPEN) + " - " + std::to_string(MAX_NODES_TO_OPEN) + "): ";
		std::cin >> nor.amount_to_open;
		std::cout << "enter amount of nodes to use: ";
		std::cin >> nor.amount_to_use;
	} while (nor.amount_to_open > MAX_NODES_TO_OPEN || nor.amount_to_open < MIN_NODES_TO_OPEN || nor.amount_to_use < MIN_NODES_TO_OPEN);
	data = SerializerRequests::serializeRequest(nor);
	data = _aes.encrypt(data);
	rr.buffer = Helper::buildRR(data, NODE_OPEN_RC, data.size());
	Helper::sendVector(_clientSocketWithDS, rr.buffer);//PROBLEM HERE SOMTIMES!!!
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

/// <summary>
/// encript data with aes layers which in aesCircuits
/// </summary>
/// <param name="data">plain text</param>
void Client::dataLayersEncription(std::vector<unsigned char>& data)
{
	std::cout << "data layer encription!!!\n";
	std::vector<AES> reverseKeys = _aesCircuitData;
	std::reverse(reverseKeys.begin(), reverseKeys.end());
	for (auto it : reverseKeys)
	{
		data = it.encrypt(data);
	}
	std::cout << "data is: " << data.data() << "\n";
}

/// <summary>
/// dencript data with aes layers which in aesCircuits
/// </summary>
/// <param name="data">cipher text</param>
void Client::dataLayersDecription(std::vector<unsigned char>& data)
{
	std::cout << "data layer decription!!!\n";
	for (auto it : _aesCircuitData)
	{
		data = it.decrypt(data);
	}
	std::cout << "data is: " << data.data() << std::endl;
}

void Client::startConversation(const bool& openNodes)
{
	char buffer[100];
	std::string domain;
	RequestInfo ri;
	RequestResult rr;
	std::vector<unsigned char> data;
	uint256_t sheredSicret;
	AES aes_tmp;
	//NODE OPPENING START
	if (openNodes)
	{
		nodeOpening();
	}
	ri = Helper::waitForResponse_AES(_clientSocketWithDS, _aes, false); //decription
	if (openNodes)
	{
		this->_passedPathGetWait = true;
	}
	unsigned int circuit_id = ri.circuit_id;
	CircuitConfirmationResponse ccr = DeserializerResponses::deserializeCircuitConfirmationResponse(ri);
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
	LinkRequest linkRequest;
	RsaKeyExchangeRequest rkeRequest;
	RsaKeyExchangeResponse rkeResponse;
	rkeRequest.public_key = _rsa.getPublicKey();
	rkeRequest.product = _rsa.getProduct();

	data = SerializerRequests::serializeRequest(rkeRequest);
	rr.buffer = Helper::buildRR(data, RSA_KEY_EXCHANGE_RC, data.size(), circuit_id);
	//to Change the make msges logic
	Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
	std::cout << "sent RSA msg\n";

	ri = Helper::waitForResponse(_clientSocketWithFirstNode);
	rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri);

	if (Status::RSA_KEY_EXCHANGE_STATUS == ri.id)
	{
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
		
		data = _rsa.Encrypt(SerializerRequests::serializeRequest(ekeRequest), nodePlaceIt->first, nodePlaceIt->second);
		
		//std::cout << "<===== FULL ENCRIPTED MSG START =====>\n";
		//for (auto it : data)
		//{
		//	std::cout << it;
		//}
		//std::cout << "\n<===== FULL ENCRIPTED MSG END =====>";
		rr.buffer = Helper::buildRR(data, ECDHE_KEY_EXCHANGE_RC, data.size(), circuit_id);
		std::cout << "ecdhe with first node msg is now sending\n";
		Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);

		ri = Helper::waitForResponse_RSA(_clientSocketWithFirstNode, _rsa);

		if (ECDHE_KEY_EXCHANGE_STATUS != ri.id)
		{
			throw std::runtime_error("Did not get ECDHE key exchange response status!");
		}
		std::cout << "ecdhe msg recved\n";

		ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri);
		_ecdhe.setG(ekeResponse.calculationResult);
		std::cout << "generate aes key!!!\n";
		sheredSicret = _ecdhe.createDefiKey();
		aes_tmp.generateRoundKeys(sheredSicret);
		_aesCircuitData.emplace_back(aes_tmp);
		std::cout << "shered sicret with first node is: " << sheredSicret << "\n";
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
	//SEND ECDHE KEY EXCHANGE TO FIRST NODE END
	
    //SENDING LINK AND RSA KEY EXCHANGE AND ECDHE KEY EXCHANGE TO ALL THE REST NODES START
	int counter = 1;
	if (ccr.nodesPath.size() > 1)
	{
		for (auto it = ccr.nodesPath.begin() + 1; it != ccr.nodesPath.end(); it++)
		{
			counter++;
			linkRequest.nextNode = std::pair<std::string, unsigned int>(it->first, stoi(it->second));
			data = SerializerRequests::serializeRequest(linkRequest);
			dataLayersEncription(data); // encript for all the previuse nodes 
			rr.buffer = Helper::buildRR(data, LINK_RC, data.size(), circuit_id);
			Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
			std::cout << "sent link msg\n";

			ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			if (Errors::LINK_ERROR == ri.id)
			{
				throw std::runtime_error("Could not build circuit.");
			}

			data = SerializerRequests::serializeRequest(rkeRequest);
			dataLayersEncription(data); // encript for all the previuse nodes 
			rr.buffer = Helper::buildRR(data, RSA_KEY_EXCHANGE_RC, data.size(), circuit_id);
			Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
			std::cout << "sent RSA msg\n";
			if(counter != 3)
				ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			dataLayersDecription(ri.buffer); //decript for all the previus nodes
			rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri);

			if (Status::RSA_KEY_EXCHANGE_STATUS == ri.id)
			{
				std::cout << "Got NODE public_key: " << rkeResponse.public_key << std::endl;
				_rsaCircuitData.emplace_back(rkeResponse.public_key, rkeResponse.product);
				nodePlaceIt++;
				std::cout << "Saved NODE pubkey " << nodePlaceIt->first << "\n";
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

				//rsa encription for ecdhe
				data = _rsa.Encrypt(SerializerRequests::serializeRequest(ekeRequest), nodePlaceIt->first, nodePlaceIt->second);
				//aes encription for the previuse nodes in the circuit
				dataLayersEncription(data); // encript for all the previuse nodes 
				rr.buffer = Helper::buildRR(data, ECDHE_KEY_EXCHANGE_RC, data.size(), circuit_id);
				Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
				
				ri = Helper::waitForResponse(_clientSocketWithFirstNode);
				dataLayersDecription(ri.buffer); //decript for all the previus nodes
				ri.buffer = _rsa.Decrypt(ri.buffer); //decript the rsa himself
				if (ECDHE_KEY_EXCHANGE_STATUS != ri.id)
				{
					throw std::runtime_error("Did not get ECDHE key exchange response status!");
				}
				std::cout << "ecdhe msg recved\n";

				ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri);
				_ecdhe.setG(ekeResponse.calculationResult);
				std::cout << "generate aes key!!!\n";
				sheredSicret = _ecdhe.createDefiKey();
				aes_tmp.generateRoundKeys(sheredSicret);
				_aesCircuitData.emplace_back(aes_tmp);
				std::cout << "shered sicret with node " << counter << " is: " << sheredSicret << "\n";
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
	//while(true) add if work
	std::cout << "Enter domain: ";
	std::cin >> domain;
	if (!domainValidationCheck(domain))
		throw std::runtime_error("domain is illegal");
	httpGetRequest.domain = domain;

	data = SerializerRequests::serializeRequest(httpGetRequest);
	dataLayersEncription(data);
	rr.buffer = Helper::buildRR(data, HTTP_MSG_RC, data.size(), circuit_id);

	Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
	std::cout << "sends httpGet Request:\n";
	ri = Helper::waitForResponse(_clientSocketWithFirstNode);
	dataLayersDecription(ri.buffer);
	HttpGetResponse httpGetResponse;
	httpGetResponse = DeserializerResponses::deserializeHttpGetResponse(ri);

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
