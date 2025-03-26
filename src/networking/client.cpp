#include "client.h"

using std::vector;

std::mutex mtx;

Client::Client(Pipe p)
{
	// we connect to server that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocketWithDS = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_clientSocketWithFirstNode = INVALID_SOCKET;
	circuit_id = 0;
	_pipe = p;
	if (_clientSocketWithDS == INVALID_SOCKET)
		throw std::runtime_error("server run error socket");
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
		_pipe.close();
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
		throw std::runtime_error("[DS CONNECTING] Cant connect to server");

	//CREATE SELF RSA PAIR OF KEYS START
	_rsa.pregenerateKeys();
	std::cout << "[DS CONNECTING] Client finished pregenerating RSA keys...\n";
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
		std::cerr << "[DS CONNECTING] Problem with RSA\n";
	}
	if (RSA_KEY_EXCHANGE_STATUS == ri.id)
	{
		RsaKeyExchangeResponse rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri);
		_serverRSA.first = rkeResponse.public_key;
		_serverRSA.second = rkeResponse.product;
		std::cout << "[DS CONNECTING] Got server's RSA public key: " << _serverRSA.first << std::endl;
	}
	else
	{
		std::cerr << "[DS CONNECTING] Could not exchange RSA keys with server!\n";
		exit(1);
	}
	//RSA KEY EXCHANGE WITH DS END

	//SEND REQUEST AND BUILD  ECDHE INFO START
	EcdheKeyExchangeRequest ekeRequest;
	EcdheKeyExchangeResponse ekeResponse;
	try
	{
		std::cout << "[DS CONNECTING] ecdhe first msg is now generating\n";
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
			throw std::runtime_error("[DS CONNECTING] Did not get ECDHE key exchange request!");
		}
		std::cout << "[DS CONNECTING] ecdhe msg recved\n";

		ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri);
		_ecdhe.setG(ekeResponse.calculationResult);
		std::cout << "[DS CONNECTING] generate aes key!!!\n";
		uint256_t sheredSicret = _ecdhe.createDefiKey();
		_aes.generateRoundKeys(sheredSicret);
		std::cout << "[DS CONNECTING] shered sicret is: " << sheredSicret << "\n";
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
	//BUILD AES KEY END
}

RequestInfo Client::nodeOpening(const bool& regular)
{
	NodeOpenRequest nor;
	RequestResult rr;
	RequestInfo ri;
	std::vector<unsigned char> data;

	std::cout << "[NODE OPENING] node openning begins\n";
	if (!regular)
	{
		rr.buffer = Helper::buildRR(NODE_OPEN_RC, circuit_id);
		Helper::sendVector(_clientSocketWithDS, rr.buffer);
		std::cout << "[NODE OPENING] send open nodes after crush\n";
		ri = Helper::waitForResponse_AES(_clientSocketWithDS, _aes, false); //decription
		return ri;
	}

	char* buffer = new char[1];
	string msg = _pipe.getMessageFromGraphics();
	nor.amount_to_open = std::stoi(msg.substr(0, msg.find(',')));
	nor.amount_to_use = std::stoi(msg.substr(msg.find(',') + 1));
	std::cout << "nor.amount_to_open: " << nor.amount_to_open << ", " << "nor.amount_to_use: " << nor.amount_to_use << std::endl;
	data = SerializerRequests::serializeRequest(nor);
	data = _aes.encrypt(data);
	rr.buffer = Helper::buildRR(data, NODE_OPEN_RC, data.size());
	Helper::sendVector(_clientSocketWithDS, rr.buffer);//PROBLEM HERE SOMTIMES!!!
	std::cout << "[NODE OPENING] Message send to server..." << std::endl;
	ri = Helper::waitForResponse_AES(_clientSocketWithDS, _aes, false); //decription
	if (ri.id == unsigned int(CIRCUIT_CONFIRMATION_STATUS))
	{
		buffer[0] = '1';
		_pipe.sendMessageToGraphics(buffer);
		return ri;
	}
	std::cout << "[NODE OPENING] input invalid! try again.\n";
	buffer[0] = '0';
	_pipe.sendMessageToGraphics(buffer);
	delete[] buffer;
	return nodeOpening(regular);
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
	std::vector<AES> reverseKeys = _aesCircuitData;
	std::reverse(reverseKeys.begin(), reverseKeys.end());
	for (auto it : reverseKeys)
	{
		data = it.encrypt(data);
	}
	std::cout << "[AES ENCRIPTION] data is: " << data.data() << "\n";
}

/// <summary>
/// dencript data with aes layers which in aesCircuits
/// </summary>
/// <param name="data">cipher text</param>
void Client::dataLayersDecription(std::vector<unsigned char>& data)
{
	for (auto it : _aesCircuitData)
	{
		data = it.decrypt(data);
	}
	std::cout << "[AES DECRIPTION] data is: " << data.data() << std::endl;
}

void Client::HandleTorClient(const bool regular)
{
	RequestInfo ri;
	RequestResult rr;
	std::vector<unsigned char> data;
	uint256_t sheredSicret;
	AES aes_tmp;
	CircuitConfirmationResponse ccr;

	try
	{
		//NODE OPPENING START
		ri = nodeOpening(regular);
		circuit_id = ri.circuit_id;

		ccr = DeserializerResponses::deserializeCircuitConfirmationResponse(ri);
		if (ccr.nodesPath.empty() || ri.id == unsigned int(CIRCUIT_CONFIRMATION_ERROR))
			throw std::runtime_error("ciruit must contains at list one node!");
		_rsaCircuitData.reserve(ccr.nodesPath.size());
		for (auto it = ccr.nodesPath.begin(); it != ccr.nodesPath.end(); it++)
			std::cout << "[HANDLER] Node: " << it->first << " " << it->second << std::endl;
		//NODE OPPENING END

		//CONNECTING TO FIRST NODE START
		_clientSocketWithFirstNode = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (_clientSocketWithFirstNode == INVALID_SOCKET)
			throw std::runtime_error("[HANDLER] Client run error socket");

		std::cout << "[HANDLER] Connecting to " << ccr.nodesPath.begin()->first << " " << ccr.nodesPath.begin()->second << std::endl;

		struct sockaddr_in sa = { 0 };
		sa.sin_port = htons(stoi(ccr.nodesPath.begin()->second));
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = inet_addr(ccr.nodesPath.begin()->first.c_str());

		if (connect(_clientSocketWithFirstNode, (struct sockaddr*)&sa, sizeof(sa)) == INVALID_SOCKET)
		{
			int errCode = WSAGetLastError();  // Get the error code from Winsock
			std::cerr << "[HANDLER] Connect failed with error code: " << errCode << std::endl;
			throw std::runtime_error("[HANDLER] Could not open socket with first node");
		}
		else std::cout << "[HANDLER] connected successfully to the first node\n";

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
		std::cout << "[HANDLER] sent RSA msg\n";

		ri = Helper::waitForResponse(_clientSocketWithFirstNode);
		rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri);

		if (Status::RSA_KEY_EXCHANGE_STATUS == ri.id)
		{
			_rsaCircuitData.emplace_back(rkeResponse.public_key, rkeResponse.product);
			std::cout << "[HANDLER] Saved FIRST NODE pubkey\n";
		}
		else
		{
			throw std::runtime_error("[HANDLER] Could not exchange RSA keys, thus could not build circuit.");
		}
		ri.buffer.clear();
		//SEND RSA KEY EXCHANGE TO FIRST NODE END

		//SEND ECDHE KEY EXCHANGE TO FIRST NODE START
		EcdheKeyExchangeRequest ekeRequest;
		EcdheKeyExchangeResponse ekeResponse;
		auto nodePlaceIt = _rsaCircuitData.begin();
		try
		{
			std::cout << "[HANDLER] ecdhe with first node msg is now generating\n";
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
			std::cout << "[HANDLER] ecdhe with first node msg is now sending\n";
			Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);

			ri = Helper::waitForResponse_RSA(_clientSocketWithFirstNode, _rsa);

			if (ECDHE_KEY_EXCHANGE_STATUS != ri.id)
			{
				throw std::runtime_error("[HANDLER] Did not get ECDHE key exchange response status!");
			}
			std::cout << "[HANDLER] ecdhe msg recved\n";

			ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri);
			_ecdhe.setG(ekeResponse.calculationResult);
			std::cout << "[HANDLER] generate aes key!!!\n";
			sheredSicret = _ecdhe.createDefiKey();
			aes_tmp.generateRoundKeys(sheredSicret);
			_aesCircuitData.emplace_back(aes_tmp);
			std::cout << "[HANDLER] shered sicret with first node is: " << sheredSicret << "\n";
		}
		catch (...)
		{
			std::cout << "[HANDLER] client crush " << std::endl;

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
				std::cout << "[HANDLER] sent link msg\n";

				ri = Helper::waitForResponse(_clientSocketWithFirstNode);
				if (Errors::LINK_ERROR == ri.id)
				{
					throw std::runtime_error("[HANDLER] Could not build circuit.");
				}

				data = SerializerRequests::serializeRequest(rkeRequest);
				dataLayersEncription(data); // encript for all the previuse nodes 
				rr.buffer = Helper::buildRR(data, RSA_KEY_EXCHANGE_RC, data.size(), circuit_id);
				Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
				std::cout << "[HANDLER] sent RSA msg\n";
				ri = Helper::waitForResponse(_clientSocketWithFirstNode);
				dataLayersDecription(ri.buffer); //decript for all the previus nodes
				rkeResponse = DeserializerResponses::deserializeRsaKeyExchangeResponse(ri);

				if (Status::RSA_KEY_EXCHANGE_STATUS == ri.id)
				{
					std::cout << "[HANDLER] Got NODE public_key: " << rkeResponse.public_key << std::endl;
					_rsaCircuitData.emplace_back(rkeResponse.public_key, rkeResponse.product);
					nodePlaceIt++;
					std::cout << "[HANDLER] Saved NODE pubkey " << nodePlaceIt->first << "\n";
				}
				else
				{
					throw std::runtime_error("[HANDLER] Could not exchange RSA keys, thus could not build circuit.");
				}

				try
				{
					std::cout << "[HANDLER] ecdhe with first node msg is now generating\n";
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
						throw std::runtime_error("[HANDLER] Did not get ECDHE key exchange response status!");
					}
					std::cout << "[HANDLER] ecdhe msg recved\n";

					ekeResponse = DeserializerResponses::deserializeEcdheKeyExchangeResponse(ri);
					_ecdhe.setG(ekeResponse.calculationResult);
					std::cout << "[HANDLER] generate aes key!!!\n";
					sheredSicret = _ecdhe.createDefiKey();
					aes_tmp.generateRoundKeys(sheredSicret);
					_aesCircuitData.emplace_back(aes_tmp);
					std::cout << "[HANDLER] shered sicret with node " << counter << " is: " << sheredSicret << "\n";
				}
				catch (...)
				{
					std::cout << "[HANDLER] client crush ";
				}
			}
		}
		//SENDING LINK AND RSA KEY EXCHANGE AND ECDHE KEY EXCHANGE TO ALL THE REST NODES END

		//SENDING HTTP GET START
		HttpGetRequest httpGetRequest;
		while (true)//add if work
		{
			std::cout << "getting msg from graphics\n";
			string msg = _pipe.getMessageFromGraphics();

			if (msg.empty()) {
				std::cout << "No message available from graphics, retrying...\n";
				std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Short delay before retry
				break;  // Skip this iteration if there's no message
			}

			int length = std::stoi(msg.substr(0, msg.find(',')));
			httpGetRequest.domain = msg.substr(msg.find(',') + 1, length);
			std::cout << "domain is: " << httpGetRequest.domain << std::endl;

			data = SerializerRequests::serializeRequest(httpGetRequest);
			dataLayersEncription(data);
			rr.buffer = Helper::buildRR(data, HTTP_MSG_RC, data.size(), circuit_id);

			Helper::sendVector(_clientSocketWithFirstNode, rr.buffer);
			std::cout << "[HANDLER] sends httpGet Request:\n";
			ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			dataLayersDecription(ri.buffer);
			HttpGetResponse httpGetResponse;
			httpGetResponse = DeserializerResponses::deserializeHttpGetResponse(ri);

			if (Errors::HTTP_MSG_ERROR == ri.id)
			{
				std::cerr << "[HANDLER] Could not get HTML of " << httpGetRequest.domain << std::endl;
				char buffer[1] = { '0' };
				if (!_pipe.sendMessageToGraphics(buffer)) 
				{
					std::cerr << "Failed to send error response back to graphics.\n";
				}
			}
			else
			{
				int i = 0;
				std::cout << "[HANDLER] HTML of " << httpGetRequest.domain << ": " << std::endl;
				string length = std::to_string(httpGetResponse.content.size());
				char* buffer = new char[length.size() + 1 + httpGetResponse.content.size()]; //switch to exact len with char* if needed 
				for(i = 0; i < length.size(); i++)
				{
					buffer[i] = length[i];
				}
				buffer[length.size()] = ',';
				for (i = length.size() + 1; i < httpGetResponse.content.size(); i++)
				{
					buffer[i] = httpGetResponse.content[i];
				}
				
				if (!_pipe.sendMessageToGraphics(buffer))
				{
					std::cerr << "Failed to send data back to graphics.\n";
				}

				delete[] buffer;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		//SENDING HTTP GET END
	}
	catch (const std::runtime_error& e)
	{
		std::cout << "[HANDLER] catched problem: " << e.what() << std::endl;
		circuit_id = 0;
		HandleTorClient();
	}
	catch (...)
	{
		std::cout << "[HANDLER] catched unexpected error\n";
		closeSocketWithFirstNode();
		_rsaCircuitData.clear();
		_aesCircuitData.clear();
		_rsa = RSA();
		_aes = AES();
		_ecdhe = ECDHE();
		std::cout << "[HANDLER] closed socket with first node\n";
		std::cout << "[HANDLER] restarting convertation\n";
		HandleTorClient(false);
	}
}

int main()
{
	try
	{
		std::cout << "Starting orbit gui...\n";
		const char* script_path = "C:\\Users\\Magshimim\\orbit\\orbit_gui\\gui.py";
		const char* python_home = "C:\\Program Files\\Python38";  // Update to the correct path where Python is installed

		// Set PYTHONHOME environment variable
		_putenv_s("PYTHONHOME", python_home);  // For MSVC
		//_putenv_s("PYTHONPATH", python_home);

		Py_Initialize();

		// Run the Python GUI script
		
		FILE* fp = nullptr;
		if (fopen_s(&fp, script_path, "r") == 0 && fp) {  // Safer fopen
			PyRun_SimpleFile(fp, script_path);
			fclose(fp);
			std::cout << "Success: orbit gui run successfully!\n";
		}
		else {
			std::cerr << "Error: Could not open gui.py at " << script_path << "\n";
		}

		// Finalize Python
		Py_Finalize();

		std::cout << "inishialize back Pipe\n";
		Pipe pip;
		srand(time_t(NULL));

		Pipe p;
		bool isConnect = p.connect(); 

		string ans;
		while (!isConnect) //reconnect manganon optional
		{
			std::cout << "cant connect to orbit graphics" << std::endl;
			std::cout << "Do you try to connect again or exit? (0-try again, 1-exit)" << std::endl;
			std::cin >> ans;

			if (ans == "0")
			{
				std::cout << "trying connect again.." << std::endl;
				Sleep(3000);
				isConnect = p.connect();
			}
			else
			{
				p.close();
				return 1;
			}
		}
		std::cout << "succesfully connected to gui pipe!\n";
		WSAInitializer wsa = WSAInitializer();
		Client client = Client(p);
		
		client.connectToServer("127.0.0.1", COMMUNICATE_SERVER_PORT);

		client.HandleTorClient();

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
