#include "server.h"
#include "handlers/TorRequestHandler.h"
#define SECONDS_TO_WAIT 10 //the maximum time we wait for node alives
#define AMOUNT_OF_BYTES 1250
// using static const instead of macros 
static const unsigned short PORT = 9787;
static const unsigned short CONTROL_PORT = 9788;
static const unsigned int IFACE = 0;

using std::string;
using std::vector;

std::mutex mutex;

DockerManager dm = DockerManager();

Server::Server()
{
	// notice that we step out to the global namespace
	// for the resolution of the function socket
	//this->dm = DockerManager();

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
	std::thread tr(&Server::clientHandler, this, client_socket);
	tr.detach();

}

void Server::clientHandler(const SOCKET client_socket)
{
	try
	{
		std::list<std::pair<std::string, std::string>> control_info;
		RequestInfo ri;
		std::string msg = "";
		RequestResult rr = RequestResult();
		mutex.lock();
		TorRequestHandler torRequestHandler = TorRequestHandler(std::ref(dm), std::ref(this->_controlList), std::ref(this->_clients));
		for (auto it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (it->second == INVALID_SOCKET)
			{
				std::cout << "new client allocated\n\n";
				it->second = client_socket;
				break;
			}
		}
		mutex.unlock(); 

		std::cout << "get msg from client " + std::to_string(client_socket) << std::endl;

		ri = Helper::waitForResponse(client_socket);
		rr = torRequestHandler.directRequest(ri);
		Helper::sendVector(client_socket, rr.buffer);
		std::cout << "sending msg...\n";
		if (static_cast<unsigned int>(rr.buffer[0]) == CIRCUIT_CONFIRMATION_ERROR)
		{
			throw std::runtime_error("failed to get nodes details");
		}
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		clientHandler(client_socket);
	}
}



void Server::serveControl() //check if its one of the nodes
{
	try
	{
		bindAndListenControl();
		std::string input_string;
		std::vector<string> clientsAlowde;
		int clientsAmount = 0; 
		while (true)
		{
			// the main thread is only accepting clients 
			// and add then to the list of handlers
			
			if (_controlList.size() > clientsAmount)
			{
				
				auto it = _controlList.begin();
				for (int i = 0; i < clientsAmount; i++)
					++it;
				
				clientsAmount += 1;
				std::cout << "accepting nodes for control from client" << clientsAmount << "...\n";
				mutex.lock();
				for (auto it = _controlList.begin(); it != _controlList.end(); it++)
				{
					for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
					{
						clientsAlowde.emplace_back(it2->first);
					}
				}
				mutex.unlock();
				this->acceptControlClient(clientsAlowde);
				mutex.lock();
				clientsAlowde.clear();
				mutex.unlock();
			}
			
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

void Server::acceptControlClient(const std::vector<string>& allowedClients)
{
	sockaddr_in nodeAddr;
	int nodeAddrLen = sizeof(nodeAddr);
	std::vector<unsigned int> circuits;
	// Accept the client connection
	SOCKET nodeSocket = accept(this->_controlSocket, (sockaddr*)&nodeAddr, &nodeAddrLen);
	if (nodeSocket == INVALID_SOCKET)
	{
		throw std::runtime_error("Failed to accept client connection.");
	}

	// Get the client's IP and port
	char nodeIP[INET_ADDRSTRLEN + INC] = { 0 };
	inet_ntop(AF_INET, &nodeAddr.sin_addr, nodeIP, INET_ADDRSTRLEN);
	nodeIP[INET_ADDRSTRLEN] = NULL;
	std::string nodeIPStr(nodeIP);

	// Check if the client is in the allowed list
	bool isAllowed = false;
	for (const auto& allowedClient : allowedClients) {
		if (allowedClient == nodeIPStr) {
			isAllowed = true;
			break;
		}
	}

	if (isAllowed)
	{
		std::cout << "Node " << nodeIPStr << " accepted." << std::endl;
		for (auto it = _controlList.begin(); it != _controlList.end(); ++it)
		{
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				if (it2->first == nodeIPStr)
				{
					circuits.emplace_back(it->first);
					break;
				}
			}
				
		}
		std::thread tr(&Server::clientControlHandler, this, nodeSocket, circuits, nodeIPStr);
		tr.detach();
	}
	else
	{
		std::cout << "Node " << nodeIPStr << " is not allowed. Closing connection." << std::endl;
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
	if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}

void Server::clientControlHandler(const SOCKET node_sock, const std::vector<unsigned int>& circuits, string nodeIp)
{
	try
	{
		DeleteCircuitRequest dcr;
		char buffer[100];
		std::vector<string> nodesCrushed;
		DWORD timeout = SECONDS_TO_WAIT * 1000; 
		setsockopt(node_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		SOCKET sockWithNode;
		while (true)
		{
			mutex.lock();
			int bytesRead = recv(node_sock, buffer, sizeof(buffer), 0); 
			mutex.unlock();
			if (bytesRead <= 0)
			{
				if (WSAGetLastError() == WSAETIMEDOUT)
				{
					//here handle
					std::cerr << "TIMEOUT: Node did not send alive message.\n";
					for (int i = 0; i < circuits.size(); i++)
					{
						std::cout << "handle circuit - " << circuits[i] << "\n\n";
						dcr.circuit_id = circuits[i];
						std::vector<unsigned char> deleteCircuitBuffer = SerializerRequests::serializeRequest(dcr);
						for (auto it = this->_controlList[circuits[i]].begin(); it != this->_controlList[circuits[i]].end(); ++it)
						{
							if (it->first == nodeIp)
							{
								nodesCrushed.emplace_back(nodeIp);
								continue;
							}
								

							sockWithNode = createSocket(it->first, static_cast<unsigned int>(std::stoi(it->second)));
							mutex.lock();
							Helper::sendVector(sockWithNode, deleteCircuitBuffer);
							mutex.unlock();
						}//delete
						mutex.lock();
						Helper::sendVector(_clients[circuits[i]], deleteCircuitBuffer);//now send to client 
						mutex.unlock();
					}
					//ajust new circuits
					//now get from client...
					
				}
				else
				{
					std::cerr << "Error receiving from socket: " << WSAGetLastError() << std::endl;
					throw std::runtime_error("Error receiving from node.");
				}
			}

			// Check if the received message is the alive message code
			if (static_cast<unsigned int>(buffer[0]) != ALIVE_MSG_RC)
			{
				std::cerr << "ERROR: Unexpected message code received. -> " << buffer[0] << "\n";
				throw std::runtime_error("Unexpected message code received.");
			} 
			// Reset the timer for next alive check
			timeout = SECONDS_TO_WAIT * 1000;
			setsockopt(node_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		}
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;

		// Close the socket and remove from control list if necessary
		closesocket(node_sock);
		// Optionally update control list to mark the node as inactive or remove it
	}
}


int main()
{
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