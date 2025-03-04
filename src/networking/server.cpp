#include "server.h"
#include "handlers/TorRequestHandler.h"
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

	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	std::thread serveControlThread(&Server::serveControl, this);

	serveClientsThread.join();
	serveControlThread.join();
}

void Server::serveClients()
{
	bindAndListen();
	std::string input_string;
	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "[CIRCUITS] accepting client...\n";
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
		throw std::runtime_error("[CIRCUITS] server bind error");
	std::cout << "[CIRCUITS] binded\n";

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error("[CIRCUITS] server listen error");
	std::cout << "[CIRCUITS] listening...\n";

}


void Server::acceptClient()
{
	SOCKET client_socket = accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::runtime_error("[CIRCUITS] invalid socket accepted");

	std::cout << "[CIRCUITS] Client accepted !\n";
	// create new thread for client	and detach from it
	//KEY EXCHANGE PROCESS WITH CLIENT START
	while (true)
	{
		_rsaInfo[client_socket].first.pregenerateKeys(); // generate rsa keys for circuit
		RequestInfo ri;
		unsigned int status = 0;
		RequestResult rr = RequestResult();

		//GET REQUEST AND BUILD RSA RESPOSE START
		RsaKeyExchangeRequest rkeRequest;
		RsaKeyExchangeResponse rkeResponse;
		try
		{
			ri = Helper::waitForResponse(client_socket);

			if (RSA_KEY_EXCHANGE_RC != ri.id)
			{
				throw std::runtime_error("[CIRCUITS] Did not get RSA key exchange request!");
			}
			std::cout << "[CIRCUITS] rsa msg was sended\n";
			rkeRequest = DeserializerRequests::deserializeRsaKeyExchangeRequest(ri);
			std::cout << "[CIRCUITS] Got public RSA key from client: " << rkeRequest.public_key << std::endl;
			_rsaInfo[client_socket].second.first = rkeRequest.public_key;
			_rsaInfo[client_socket].second.second = rkeRequest.product;

			rkeResponse.public_key = this->_rsaInfo[client_socket].first.getPublicKey();
			rkeResponse.product = this->_rsaInfo[client_socket].first.getProduct();
			status = RSA_KEY_EXCHANGE_STATUS;
		}
		catch (...)
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
				throw std::runtime_error("[CIRCUITS] Did not get ECDHE key exchange request!");
			}
			std::cout << "[CIRCUITS] ecdhe msg recved\n";
			ekeRequest = DeserializerRequests::deserializeEcdheKeyExchangeRequest(ri);
			_ecdheInfo[client_socket].setG(ekeRequest.b);
			_ecdheInfo[client_socket].setP(ekeRequest.m);
			_ecdheInfo[client_socket].createTmpKey();
			ekeResponse.calculationResult = _ecdheInfo[client_socket].createDefiKey();
		}
		catch (...)
		{
			status = ECDHE_KEY_EXCHANGE_ERROR;
			break;
		}
		try
		{
			data = _rsaInfo[client_socket].first.Encrypt(SerializerResponses::serializeResponse(ekeResponse), _rsaInfo[client_socket].second.first, _rsaInfo[client_socket].second.second);
			rr.buffer = Helper::buildRR(data, status, data.size());

			std::cout << "[CIRCUITS] ecdhe msg sended\n";
			Helper::sendVector(client_socket, rr.buffer);
			//GET REQUEST AND BUILD ECDHE INFO END

			//BUILD AES KEY START
			_ecdheInfo[client_socket].setG(ekeRequest.calculationResult);
			std::cout << "[CIRCUITS] generate aes key!!!\n";
			uint256_t sheredSicret = _ecdheInfo[client_socket].createDefiKey();
			_aes.generateRoundKeys(sheredSicret);
			std::cout << "[CIRCUITS] shered sicret is: " << sheredSicret << "\n";
		}
		catch (...)
		{
			break;
		}
		//BUILD AES KEY END
		//KEY EXCHANGE PROCESS WITH CLIENT END 
		
		std::thread tr(&Server::clientHandler, this, client_socket);
		tr.detach();
		break;
	}
}

void Server::clientHandler(const SOCKET client_socket)
{
	try
	{
		RequestInfo ri;
		unsigned int circuit_id = 0, status = 0;
		RequestResult rr = RequestResult();

		char error = 0;
		socklen_t len = sizeof(error);
		int retval = getsockopt(client_socket, SOL_SOCKET, SO_ERROR, &error, &len);
		if (retval != 0 || error != 0) {
			std::cout << "[CLIENT] Socket is not valid!\n";
		}

		//PREPER TOR REQUEST HANDLER START
		mutex.lock();
		std::cout << "[CIRCUITS] inishialize tor request handler\n";
		mutex.unlock();
		// new Client circuit : INVALID_SOCKET 
		//PREPER TOR REQUEST HANDLER END
		TorRequestHandler torRequestHandler = TorRequestHandler(std::ref(dm), std::ref(this->_controlList), std::ref(this->_clients), std::ref(_aes));
		//SEND CIRCUIT INFO START
		mutex.lock();
		std::cout << "[CIRCUITS] get msg from client " + std::to_string(client_socket) << std::endl;
		mutex.unlock();
		ri = Helper::waitForResponse(client_socket);

		rr = torRequestHandler.directRequest(ri);

		for (auto it : _clients)
		{
			if (it.second == INVALID_SOCKET)
			{
				std::lock_guard<std::mutex> lock(mutex);
				circuit_id = it.first;
				_clients[circuit_id] = client_socket;
				std::cout << "[CIRCUITS] new client allocated\n\n";
				break;
			}
		}
		Helper::sendVector(client_socket, rr.buffer);
		mutex.lock();
		std::cout << "[CIRCUITS] sending msg...\n";
		mutex.unlock();
		if (static_cast<unsigned int>(rr.buffer[1]) == CIRCUIT_CONFIRMATION_ERROR)
		{
			throw std::runtime_error("[CIRCUITS] failed to get nodes details");
		}
		//SEND CIRCUIT INFO END
		clientHandler(client_socket);
	}
	catch (...)
	{
		std::lock_guard<std::mutex> lock(mutex);
		std::cout << "[CIRCUITS] client handler crushes!!!";
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
				mutex.lock();
				std::cout << "[CONTROL] accepting nodes from " << clientsAmount << " amount of client...\n";
				mutex.unlock();
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
		throw std::runtime_error("[CONTROL] server bind error");
	std::cout << "[CONTROL] binded\n";

	if (::listen(this->_controlSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error("[CONTROL] server listen error");
	std::cout << "[CONTROL] listening...\n";
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
		throw std::runtime_error("[CONTROL] Failed to accept client connection.");
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
	for (auto it : _controlList)
	{
		for (auto it2 : it.second)
		{
			AlowdeNodes.emplace_back(it2.first);
		}
	}
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
		std::cout << "[CONTROL] Node " << nodeIPStr << " accepted." << std::endl;
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
		std::lock_guard<std::mutex> lock(mutex);
		std::cout << "[CONTROL] Node " << nodeIPStr << " is not allowed. Closing connection." << std::endl;
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

void Server::clientControlHandler(const SOCKET& node_sock, const std::vector<unsigned int>& circuits, std::string nodeIp)
{
	try
	{
		mutex.lock();
		std::cout << "[CONTROL] Enter to control with socket " << node_sock << std::endl;
		mutex.unlock();
		while (true)
		{
			if (handleCircuitNotifications(circuits, nodeIp, std::ref(node_sock)))
				return;

			if (!receiveAliveMessage(std::ref(node_sock), nodeIp))
			{
				handleNodeTimeout(circuits, nodeIp, std::ref(node_sock));
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		closesocket(node_sock);
	}
	catch (...)
	{
		std::cout << "[CONTROL] Unexpected problem caught!\n";
	}
}

void Server::setupSocketTimeout(const SOCKET& node_sock, int seconeds_to_wait)
{
	std::lock_guard<std::mutex> lock(mutex);
	std::cout << "[CONTROL] set time out of " << seconeds_to_wait << " seconds\n";
	DWORD timeout = DWORD(seconeds_to_wait * 1000);
	setsockopt(node_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
}

bool Server::handleCircuitNotifications(const std::vector<unsigned int>& circuits, const std::string& nodeIp, const SOCKET& node_sock)
{
	std::unique_lock<std::mutex> lock(circuitMutex);

	circuitCondition.wait_for(lock, std::chrono::milliseconds(SECONDS_TO_WAIT * 500), [&]() {
		return checkNotifications(circuits, nodeIp);
		});

	return processCircuitNotifications(circuits, nodeIp, node_sock);
}

bool Server::checkNotifications(const std::vector<unsigned int>& circuits, const std::string& nodeIp)
{
	for (unsigned int circuitId : circuits)
	{
		if (_circuitsToNotify.count(circuitId) && _circuitsToNotify[circuitId].count(nodeIp))
		{
			return true;
		}
	}
	return false;
}

bool Server::processCircuitNotifications(const std::vector<unsigned int>& circuits, const std::string& nodeIp, const SOCKET& node_sock)
{
	for (unsigned int circuitId : circuits)
	{
		if (_circuitsToNotify.count(circuitId) && _circuitsToNotify[circuitId].count(nodeIp))
		{
			_circuitsToNotify[circuitId].erase(nodeIp);
			if (_circuitsToNotify[circuitId].empty())
			{
				_circuitsToNotify.erase(circuitId);
			}
			mutex.lock();
			std::cout << "[CONTROL] this node has notified\n";
			mutex.unlock();
			notifyNodeDeletion(node_sock, circuitId); //stays as is
			//notifyClientDeletion(circuitId); no need
			regenerateCircuit(circuitId, nodeIp); //changed
			return true;
		}
	}
	mutex.lock();
	std::cout << "[CONTROL] no notifications\n";
	mutex.unlock();
	return false;
}

void Server::notifyNodeDeletion(const SOCKET& node_sock, unsigned int circuitId)
{
	RequestResult rr;
	rr.buffer = Helper::buildRR(DELETE_CIRCUIT_RC, circuitId);
	Helper::sendVector(node_sock, rr.buffer);
	mutex.lock();
	std::cout << "[CONTROL] Node notified for circuit " << circuitId << ".\n";
	mutex.unlock();
}

//void Server::notifyClientDeletion(unsigned int circuitId)
//{
//	RequestResult rr;
//	rr.buffer = Helper::buildRR(DELETE_CIRCUIT_RC, circuitId);
//	Helper::sendVector(_clients[circuitId], rr.buffer);
//	std::cerr << "[CONTROL] Client notified for circuit " << circuitId << ".\n";
//}

void Server::regenerateCircuit(unsigned int circuitId, const std::string& nodeIp)
{
	std::vector<std::pair<std::string, std::string>> newCircuit = dm.giveCircuitAfterCrush(nodeIp, _controlList[circuitId].size(), circuitId);
	_controlList[circuitId] = newCircuit;
	std::cout << "[CONTROL] Circuit " << circuitId << " regenerated.\n";
}

bool Server::receiveAliveMessage(const SOCKET& node_sock, const std::string& nodeIp)
{
	try
	{
		setupSocketTimeout(node_sock);
		RequestInfo ri = Helper::waitForResponse(node_sock);
		if (ri.id != ALIVE_MSG_RC)
		{
			std::cerr << "[CONTROL] ERROR: Unexpected message code received -> " << ri.id << "\n";
			throw std::runtime_error("[CONTROL] Unexpected message code received.");
		}
		//std::cout << "[CONTROL] Successfully recved alive msg!!!\n";
		return true;
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		return false;
	}
}

void Server::handleNodeTimeout(const std::vector<unsigned int>& circuits, const std::string& nodeIp, const SOCKET& node_sock)
{
	mutex.lock();
	std::cerr << "[CONTROL] Node " << nodeIp << " did not send alive message.\n";
	mutex.unlock();
	for (unsigned int circuitId : circuits)
	{
		std::unique_lock<std::mutex> lock(circuitMutex);
		_circuitsToNotify[circuitId].insert(nodeIp);
		circuitCondition.notify_all();
	}
	setupSocketTimeout(node_sock);
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
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}
	system("pause");
	return 0;
}