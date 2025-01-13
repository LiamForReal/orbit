#include "client.h"
using std::vector;

std::mutex mtx;

Client::Client()
{
	this->rsa.pregenerateKeys();
	std::cout << "Client finished pregenerating RSA keys...\n";

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
		this->rsaCircuitData.clear();
		closesocket(_clientSocketWithFirstNode);
		closesocket(_clientSocketWithDS);
	}
	catch (...) {}
}

void Client::connectToServer(std::string serverIP, int port)
{

	struct sockaddr_in sa = { 0 };
	RequestInfo ri;
	vector<unsigned char> tmp, data;
	sa.sin_port = htons(port); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());    // the IP of the server

	// the process will not continue until the server accepts the client
	int status = connect(_clientSocketWithDS, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::runtime_error("Cant connect to server");

	RsaKeyExchangeRequest rkeRequest;
	rkeRequest.public_key = this->rsa.getPublicKey();
	rkeRequest.product = this->rsa.getProduct();
	tmp = SerializerRequests::serializeRequest(rkeRequest);
	data.emplace_back(0); // temporery circuit id!!!
	std::cout << "\nis this where the code crush?\n"; //YESSSS
	std::copy(tmp.begin(), tmp.end(), data.begin() + 1);
	try
	{
		Helper::sendVector(_clientSocketWithDS, data);
		ri = Helper::waitForResponse(_clientSocketWithDS);
	}
	catch (...)
	{
		std::cerr << "Problem with RSA\n";
	}
	if (RSA_KEY_EXCHANGE_STATUS == ri.id)
	{
		RsaKeyExchangeResponse rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri.buffer);
		this->rsaServerPubkey = rkeResponse.public_key;
		this->rsaServerProduct = rkeResponse.product;
		std::cout << "Got server's RSA public key: " << this->rsaServerPubkey << std::endl;
	}
	else
	{
		std::cerr << "Could not exchange RSA keys with server!\n";
		exit(1);
	}
}

void Client::nodeOpening()
{
	NodeOpenRequest nor;
	std::vector<unsigned char> tmp;
	std::vector<unsigned char> data;
	do
	{
		std::cout << "enter amount of nodes to open (between " + std::to_string(MIN_NODES_TO_OPEN) + " - " + std::to_string(MAX_NODES_TO_OPEN) + "): ";
		std::cin >> nor.amount_to_open;
		std::cout << "enter amount of nodes to use: ";
		std::cin >> nor.amount_to_use;
	} while (nor.amount_to_open > MAX_NODES_TO_OPEN || nor.amount_to_open < MIN_NODES_TO_OPEN || nor.amount_to_use < MIN_NODES_TO_OPEN);
	data.emplace_back(0); // temporery circuit id = 0 default!!!
	tmp = SerializerRequests::serializeRequest(nor);
	std::copy(tmp.begin(), tmp.end(), data.begin() + 1);
	Helper::sendVector(_clientSocketWithDS, data);
	std::cout << "Message send to server..." << std::endl;

	//CircuitConfirmationResponse ccr = DeserializerResponses::deserializeCircuitConfirmationResponse();
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
	//NODE OPPENING START
	if (openNodes)
	{
		nodeOpening();
	}

	ri = Helper::waitForResponse(this->_clientSocketWithDS);

	if (openNodes)
	{
		this->_passedPathGetWait = true;
	}

	CircuitConfirmationResponse ccr = DeserializerResponses::deserializeCircuitConfirmationResponse(ri.buffer);

	this->rsaCircuitData.reserve(ccr.nodesPath.size());

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
	int status = connect(_clientSocketWithFirstNode, (struct sockaddr*)&sa, sizeof(sa));
	if (status == INVALID_SOCKET)
	{
		int errCode = WSAGetLastError();  // Get the error code from Winsock
		std::cerr << "Connect failed with error code: " << errCode << std::endl;
		throw std::runtime_error("Could not open socket with first node");
	}
	else std::cout << "connected successfully to the first node\n";

	//CONNECTING TO FIRST NODE END

	//SEND RSA KEY EXCHANGE TO FIRST NODE START
	std::vector<unsigned char> tmp;
	std::vector<unsigned char> dataRKE;
	std::vector<unsigned char> data;
	LinkRequest linkRequest;
	RsaKeyExchangeRequest rkeRequest;
	RsaKeyExchangeResponse rkeResponse;
	rkeRequest.public_key = rsa.getPublicKey();
	rkeRequest.product = rsa.getProduct();
	tmp = SerializerRequests::serializeRequest(rkeRequest);
	dataRKE.emplace_back(ccr.circuit_id);
	//to Change the make msges logic
	std::copy(tmp.begin(), tmp.end(), dataRKE.begin() + 1);
	Helper::sendVector(_clientSocketWithFirstNode, dataRKE);
	std::cout << "sent RSA msg\n";

	ri = Helper::waitForResponse(_clientSocketWithFirstNode);
	rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri.buffer);

	if (Status::RSA_KEY_EXCHANGE_STATUS == rkeResponse.status)
	{
		std::cout << "Got FIRST NODE public_key: " << rkeResponse.public_key << std::endl;
		this->rsaCircuitData.emplace_back(rkeResponse.public_key, rkeResponse.product);
		std::cout << "Saved FIRST NODE pubkey\n";
	}
	else
	{
		throw std::runtime_error("Could not exchange RSA keys, thus could not build circuit.");
	}
	ri.buffer.clear();
	//SEND RSA KEY EXCHANGE TO FIRST NODE END
	
    //SENDING LINK AND RSA KEY EXCHANGE TO ALL THE REST NODES START
	if (ccr.nodesPath.size() > 1)
	{
		for (auto it = ccr.nodesPath.begin(); it != ccr.nodesPath.end(); it++)
		{
			if (it == ccr.nodesPath.begin())
				it++;

			linkRequest.nextNode = std::pair<std::string, unsigned int>(it->first, stoi(it->second));
			data.clear();
			data.emplace_back(ccr.circuit_id);
			tmp = SerializerRequests::serializeRequest(linkRequest);
			std::copy(tmp.begin(), tmp.end(), data.begin() + 1);
			Helper::sendVector(_clientSocketWithFirstNode, data);
			std::cout << "sent link msg\n";

			ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			if (Errors::LINK_ERROR == ri.id)
			{
				throw std::runtime_error("Could not build circuit.");
			}
			ri.buffer.clear();


			Helper::sendVector(_clientSocketWithFirstNode, dataRKE);
			std::cout << "sent RSA msg\n";
			ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			//ri = Helper::waitForResponse_RSA(_clientSocketWithFirstNode, std::ref(this->rsa));
			rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri.buffer);

			if (Status::RSA_KEY_EXCHANGE_STATUS == rkeResponse.status)
			{
				std::cout << "Got NODE public_key: " << rkeResponse.public_key << std::endl;
				this->rsaCircuitData.emplace_back(rkeResponse.public_key, rkeResponse.product);
				std::cout << "Saved NODE pubkey\n";
			}
			else
			{
				throw std::runtime_error("Could not exchange RSA keys, thus could not build circuit.");
			}
			ri.buffer.clear();
		}
	}
	//SENDING LINK AND RSA KEY EXCHANGE TO ALL THE REST NODES END

	//SENDING HTTP GET START
	HttpGetRequest httpGetRequest;
	data.clear();

	std::cout << "Enter domain: ";
	std::cin >> domain;
	if (!domainValidationCheck(domain))
		throw std::runtime_error("domain is illegal");

	data.emplace_back(ccr.circuit_id);
	httpGetRequest.domain = domain;
	tmp = SerializerRequests::serializeRequest(httpGetRequest);
	std::copy(tmp.begin(), tmp.end(), data.begin() + 1);

	Helper::sendVector(_clientSocketWithFirstNode, data);
	std::cout << "sends httpGet Request:\n";
	ri = Helper::waitForResponse(_clientSocketWithFirstNode);

	HttpGetResponse httpGetResponse;
	httpGetResponse = DeserializerResponses::deserializeHttpGetResponse(ri.buffer);

	if (Errors::HTTP_MSG_ERROR == httpGetResponse.status)
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
				std::thread listenToServerInfoThread(&Client::listenToServerInfo, std::ref(client));
				listenToServerInfoThread.detach();
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
