#include "server.h"
#include "handlers/TorRequestHandler.h"
#define SECONDS_TO_WAIT 10 //the maximum time we wait for node alives
// using static const instead of macros 
static const unsigned short PORT = 9787;
static const unsigned short CONTROL_PORT = 9788;
static const unsigned int IFACE = 0;

using std::string;
using std::vector;

std::mutex mutex;
std::condition_variable circuitCondition;
std::mutex circuitMutex;
DockerManager dm = DockerManager();

Server::Server()
{
	std::cout << "Server finished pregenerating RSA keys...\n";
	
	// notice that we step out to the global namespace
	// for the resolution of the function socket
	//this->dm = DockerManager();
	_aes = AES();
	this->_circuitsToNotify = std::map<unsigned int, std::set<string>>();
	_controlList = std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>();

	_socket = socket(AF_INET,  SOCK_STREAM,  IPPROTO_TCP); 
	if (_socket == INVALID_SOCKET)
		throw std::runtime_error("server run error socket");

	_controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_controlSocket == INVALID_SOCKET)
		throw std::runtime_error("server run error control socket");

}

Server::~Server()
{
	std::cout << "closing accepting socket\n";
	// why is this try necessarily ?
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		::closesocket(_socket);
	}
	catch (...) {}
}

void Server::serve()
{
	std::thread serveClientsThread(&Server::serveClients, this);
	//std::thread serveControlThread(&Server::serveControl, this); - control

	serveClientsThread.join();
	//serveControlThread.join();
}

void Server::serveClients()
{
	bindAndListen();
	std::string input_string;
	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "accepting client...\n" ;
		acceptClient();

	}
}

// listen to connecting requests from clients
// accept them, and create thread for each client
void Server::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(unsigned int(PORT));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	// again stepping out to the global namespace
	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::runtime_error("server bind error");
	std::cout << "binded\n";

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error("server listen error");
	std::cout << "listening...\n";

}


void Server::acceptClient()
{
	SOCKET client_socket = accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::runtime_error("invalid socket accepted");

	std::cout << "Client accepted !\n";
	// create new thread for client	and detach from it

	_rsaInfo[client_socket].first.pregenerateKeys(); // generate rsa keys for circuit
	RequestInfo ri;
	unsigned int circuit_id = 0, status = 0;
	RequestResult rr = RequestResult();


	//GET REQUEST AND BUILD RSA RESPOSE START
	RsaKeyExchangeRequest rkeRequest;
	RsaKeyExchangeResponse rkeResponse;
	try
	{
		ri = Helper::waitForResponse(client_socket);

		if (RSA_KEY_EXCHANGE_RC != ri.id)
		{
			throw std::runtime_error("Did not get RSA key exchange request!");
		}
		std::cout << "rsa msg was sended\n";
		rkeRequest = DeserializerRequests::deserializeRsaKeyExchangeRequest(ri);
		std::cout << "Got public RSA key from client: " << rkeRequest.public_key << std::endl;
		_rsaInfo[client_socket].second.first = rkeRequest.public_key;
		_rsaInfo[client_socket].second.second = rkeRequest.product;

		rkeResponse.public_key = this->_rsaInfo[client_socket].first.getPublicKey();
		rkeResponse.product = this->_rsaInfo[client_socket].first.getProduct();
		status = RSA_KEY_EXCHANGE_STATUS;
	}
	catch (std::runtime_error e)
	{
		status = RSA_KEY_EXCHANGE_ERROR;
	}
	//GET REQUEST AND BUILD RSA RESPOSE END
	std::vector<unsigned char> data = SerializerResponses::serializeResponse(rkeResponse);
	rr.buffer = Helper::buildRR(data, status, data.size());
	Helper::sendVector(client_socket, rr.buffer);


	//GET REQUEST AND BUILD  ECDHE INFO START
	EcdheKeyExchangeRequest ekeRequest;
	EcdheKeyExchangeResponse ekeResponse;
	uint256_t tmpKey;
	try
	{
		status = ECDHE_KEY_EXCHANGE_STATUS;
		ri = Helper::waitForResponse_RSA(client_socket, _rsaInfo[client_socket].first);

		if (ECDHE_KEY_EXCHANGE_RC != ri.id)
		{
			throw std::runtime_error("Did not get ECDHE key exchange request!");
		}
		std::cout << "ecdhe msg recved\n";
		ekeRequest = DeserializerRequests::deserializeEcdheKeyExchangeRequest(ri);
		_ecdheInfo[client_socket].setG(ekeRequest.b);
		_ecdheInfo[client_socket].setP(ekeRequest.m);
		_ecdheInfo[client_socket].createTmpKey();
		ekeResponse.calculationResult = _ecdheInfo[client_socket].createDefiKey();
	}
	catch (std::runtime_error e)
	{
		status = ECDHE_KEY_EXCHANGE_ERROR;
	}
	try
	{
		data = _rsaInfo[client_socket].first.Encrypt(SerializerResponses::serializeResponse(ekeResponse), _rsaInfo[client_socket].second.first, _rsaInfo[client_socket].second.second);
		rr.buffer = Helper::buildRR(data, status, data.size());

		std::cout << "ecdhe msg sended\n";
		Helper::sendVector(client_socket, rr.buffer);
		//GET REQUEST AND BUILD ECDHE INFO END

		//BUILD AES KEY START
		_ecdheInfo[client_socket].setG(ekeRequest.calculationResult);
		std::cout << "generate aes key!!!\n";
		uint256_t sheredSicret = _ecdheInfo[client_socket].createDefiKey();
		_aes.generateRoundKeys(sheredSicret);
		std::cout << "shered sicret is: " << sheredSicret << "\n";
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
	std::thread tr(&Server::clientHandler, this, client_socket);
	tr.detach();

}

void Server::clientHandler(const SOCKET client_socket)
{
	try
	{
		std::list<std::pair<std::string, std::string>> control_info;
		RequestInfo ri;
		unsigned int circuit_id = 0, status = 0;
		RequestResult rr = RequestResult();
		//BUILD AES KEY END
		//PREPER TOR REQUEST HANDLER START
		mutex.lock();
		TorRequestHandler torRequestHandler = TorRequestHandler(std::ref(dm), std::ref(this->_controlList), std::ref(this->_clients), std::ref(_aes)); // new Client circuit : INVALID_SOCKET 
		mutex.unlock(); 
		//PREPER TOR REQUEST HANDLER END

		//SEND CIRCUIT INFO START
		std::cout << "get msg from client " + std::to_string(client_socket) << std::endl;
		ri = Helper::waitForResponse_AES(client_socket, _aes, false);
		rr = torRequestHandler.directRequest(ri);
		mutex.lock();
		for (auto it : _clients)
		{
			if (it.second == INVALID_SOCKET)
			{
				circuit_id = it.first;
				_clients[circuit_id] = client_socket;
				
				std::cout << "new client allocated\n\n";
				break;
			}
		}
		mutex.unlock();
		Helper::sendVector(client_socket, rr.buffer);
		std::cout << "sending msg...\n";
		if (static_cast<unsigned int>(rr.buffer[1]) == CIRCUIT_CONFIRMATION_ERROR)
		{
			throw std::runtime_error("failed to get nodes details");
		}
		//SEND CIRCUIT INFO END
	}
	catch (const std::runtime_error& e)
	{
		std::cout << "client handler crushes!!!";
		std::cerr << e.what() << std::endl;
		std::cout << "rerun him\n";
		clientHandler(client_socket);
	}
}



void Server::serveControl() //check if its one of the nodes
{
	try
	{
		bindAndListenControl();
		int clientsAmount = 0; 
		while (true)
		{
			// the main thread is only accepting clients 
			// and add then to the list of handlers
			//client -> nodes 
			if (!_controlList.empty())
			{
				std::cout << "accepting nodes for control from client" << clientsAmount << "...\n";
				this->acceptControlClient(); //give all the exisiting nodes
			}
			
			// IMPORTANT NOTE FOR FUTURE DEBUGGING [25.12.2024]:
			// THE ASSIGNMENTS OF CLIENTS ALLOWED COULD AND HAPPENING
			// AFTER _CONTROL_LIST IS UPDATED, AND IT FORGETS THE NEW NODES IN THE CIRCUIT,
			// OR IN OUR CASE THE NEW NODE'S IP.
			// * TO FIX THAT - TO GIVE ACCEPTCONTROLCLIENT NOTHING (VOID) AND MAKE THIS FUNCTION
			// AFTER ACCEPTING CLIENTS AND BEFORE CHECK ITS VALIDATION,
			// TO CREATE THIS STRUCTURE OF CLIENTS ALLOWED.
		}
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void Server::bindAndListenControl()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(unsigned int(CONTROL_PORT));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	if (::bind(this->_controlSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::runtime_error("server control bind error");
	std::cout << "binded control\n";

	if (::listen(this->_controlSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error("server control listen error");
	std::cout << "listening control...\n";
}

void Server::acceptControlClient()
{
	sockaddr_in nodeAddr;
	std::vector<string> AlowdeNodes;
	int nodeAddrLen = sizeof(nodeAddr);
	std::vector<unsigned int> circuits;
	// Accept the client connection
	SOCKET nodeSocket = accept(this->_controlSocket, (sockaddr*)&nodeAddr, &nodeAddrLen);//wait here
	if (nodeSocket == INVALID_SOCKET)
	{
		throw std::runtime_error("Failed to accept client connection.");
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	mutex.lock(); // update
	for (auto it : _controlList)
	{
		for (auto it2 : it.second)
		{
			AlowdeNodes.emplace_back(it2.first);
		}
	}
	mutex.unlock();

	// Get the client's IP and port
	char nodeIP[INET_ADDRSTRLEN + INC] = { 0 };
	inet_ntop(AF_INET, &nodeAddr.sin_addr, nodeIP, INET_ADDRSTRLEN);
	nodeIP[INET_ADDRSTRLEN] = NULL;
	std::string nodeIPStr(nodeIP);

	// Check if the client is in the allowed list
	bool isAllowed = false;
	for (auto allowedClient : AlowdeNodes) 
	{
		if (allowedClient == nodeIPStr) 
		{
			isAllowed = true;
			break;
		}
	}

	if (isAllowed)
	{
		mutex.lock();
		std::cout << "Node " << nodeIPStr << " accepted." << std::endl;
		mutex.unlock();
		for (auto it : _controlList)
		{
			for (auto it2 : it.second)
			{
				if (it2.first == nodeIPStr)
				{
					circuits.emplace_back(it.first); //take all the circuits this node is part of 
					break;
				}
			}
		}
		std::thread tr(&Server::clientControlHandler, this, nodeSocket, circuits, nodeIPStr);
		tr.detach();
	}
	else
	{
		mutex.lock();
		std::cout << "Node " << nodeIPStr << " is not allowed. Closing connection." << std::endl;
		mutex.unlock();
		closesocket(nodeSocket);
	}

}

SOCKET Server::createSocket(const std::string& ip, unsigned int port)
{
	SOCKET sock = INVALID_SOCKET;

	// Create the socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
		return INVALID_SOCKET;
	}

	// Set up the sockaddr_in structure
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port); // Convert to network byte order
	if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
		std::cerr << "Invalid IP address format" << std::endl;
		closesocket(sock);
		return INVALID_SOCKET;
	}

	// Connect to the server
	if (::connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}

void Server::clientControlHandler(const SOCKET node_sock, const std::vector<unsigned int>& circuits, std::string nodeIp)
{
	try
	{
		char buffer[100];
		RequestInfo ri;
		DWORD timeout = SECONDS_TO_WAIT * 1000;
		RequestResult rr;
		setsockopt(node_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		std::cout << "Enter to control with socket " << node_sock << std::endl;

		while (true)
		{
			// Wait for notifications or timeout
			{
				std::unique_lock<std::mutex> lock(circuitMutex);

				// Check circuit-specific notifications for the current node
				circuitCondition.wait_for(lock, std::chrono::milliseconds(SECONDS_TO_WAIT * 500), [&]()
					{
						for (unsigned int circuitId : circuits)
						{
							// Ensure only affected circuits for the current node are checked
							if (_circuitsToNotify.count(circuitId) && _circuitsToNotify[circuitId].count(nodeIp))
							{
								return true; // Break when a relevant notification is found
							}
						}
						return false; // No relevant notifications
					});

				// Process circuit notifications if any
				for (unsigned int circuitId : circuits)
				{
					if (_circuitsToNotify.count(circuitId) && _circuitsToNotify[circuitId].count(nodeIp))
					{
						// Remove the node from the notifications
						_circuitsToNotify[circuitId].erase(nodeIp);
						if (_circuitsToNotify[circuitId].empty())
						{
							_circuitsToNotify.erase(circuitId); // Clean up if no more nodes are affected
						}

						// Handle delete circuit logic
						rr.buffer = Helper::buildRR(DELETE_CIRCUIT_RC, circuitId);
						// Notify the remaining nodes
						Helper::sendVector(node_sock, rr.buffer);
						std::cerr << "Node " << nodeIp << " notified for circuit " << circuitId << ".\n";

						std::this_thread::sleep_for(std::chrono::milliseconds(500));

						Helper::sendVector(_clients[circuitId], rr.buffer);
						std::cerr << "Client " << _clients[circuitId] << " notified for circuit " << circuitId << ".\n";

						// Regenerate the circuit for the remaining nodes
						std::vector<std::pair<std::string, std::string>> newCircuit = dm.giveCircuitAfterCrush(nodeIp, _controlList[circuitId].size(), circuitId);
						_controlList[circuitId] = newCircuit;

						CircuitConfirmationResponse ccr;
						ccr.nodesPath = newCircuit;

						std::vector<unsigned char> data = SerializerResponses::serializeResponse(ccr);
						rr.buffer = Helper::buildRR(data, CIRCUIT_CONFIRMATION_STATUS, data.size(), circuitId);
						Helper::sendVector(_clients[circuitId], rr.buffer);

						std::cerr << "Node " << nodeIp << " regenerated and circuit updated for circuit " << circuitId << ".\n";
						return; //close this socket does not exsist
					}
				}
			}

			// Receive alive message
			mutex.lock();
			int bytesRead = recv(node_sock, buffer, sizeof(buffer), 0);
			mutex.unlock();

			if (bytesRead <= 0)
			{
				if (WSAGetLastError() == WSAETIMEDOUT)
				{
					std::cerr << "TIMEOUT: Node " << nodeIp << " did not send alive message.\n";

					for (unsigned int circuitId : circuits)
					{
						std::unique_lock<std::mutex> lock(circuitMutex);

						// Add the failed node to the notification list
						_circuitsToNotify[circuitId].insert(nodeIp);

						// Notify all threads handling this circuit
						circuitCondition.notify_all();
					}

					timeout = SECONDS_TO_WAIT * 1000;
					setsockopt(node_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
				}
				else
				{
					std::cerr << "Error receiving from socket: " << WSAGetLastError() << std::endl;
					throw std::runtime_error("Error receiving from node.");
				}
			}

			// Validate the received message
			if (static_cast<unsigned int>(buffer[0]) != ALIVE_MSG_RC)
			{
				std::cerr << "ERROR: Unexpected message code received. -> " << buffer[0] << "\n";
				throw std::runtime_error("Unexpected message code received.");
			}

			//std::cout << "Alive sent (" << nodeIp << ")\n";

			timeout = SECONDS_TO_WAIT * 1000;
			setsockopt(node_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		}
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		closesocket(node_sock);
	}
	catch (...)
	{
		std::cout << "Unexpected problem caught!\n";
	}
}

int main()
{
	srand(time(NULL));

    try
    {
        WSAInitializer wsa = WSAInitializer();
        Server server = Server(); 
        server.serve(); //RUN ON THREAD 
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    system("pause");
    return 0; 
}