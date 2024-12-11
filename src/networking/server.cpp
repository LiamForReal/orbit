#include "server.h"
#include "handlers/TorRequestHandler.h"

#define AMOUNT_OF_BYTES 1250
// using static const instead of macros 
static const unsigned short PORT = 9787;
static const unsigned short CONTROL_PORT = 9788;
static const unsigned int IFACE = 0;

using std::string;
using std::vector;

DockerManager dm = DockerManager();

Server::Server()
{
	// notice that we step out to the global namespace
	// for the resolution of the function socket
	_socket = socket(AF_INET,  SOCK_STREAM,  IPPROTO_TCP); 
	if (_socket == INVALID_SOCKET)
		throw std::runtime_error("server run error socket");
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

void Server::serveControl(std::list<std::pair<std::string, std::string>>& control_info) //check if its one of the nodes
{
	bindAndListenControl();
	std::string input_string;
	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "accepting node for control...\n";
		this->acceptControlClient(control_info);

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

void Server::bindAndListenControl()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(unsigned int(CONTROL_PORT));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	// again stepping out to the global namespace
	if (::bind(this->_controlSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::runtime_error("server bind error");
	std::cout << "binded control\n";

	if (::listen(this->_controlSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error("server listen error");
	std::cout << "listening control...\n";
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

void Server::acceptControlClient(const std::list<std::pair<std::string, std::string>>& allowedClients)
{
	sockaddr_in nodeAddr;
	int nodeAddrLen = sizeof(nodeAddr);

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
	std::string clientIPStr(nodeIP);
	std::string clientPortStr = std::to_string(ntohs(nodeAddr.sin_port)); // Get port as string

	// Check if the client is in the allowed list
	bool isAllowed = false;
	for (const auto& allowedClient : allowedClients) {
		if (allowedClient.first == clientIPStr && allowedClient.second == clientPortStr) {
			isAllowed = true;
			break;
		}
	}

	if (isAllowed)
	{
		std::cout << "Client " << clientIPStr << ":" << clientPortStr << " accepted." << std::endl;
		std::thread tr(&Server::clientControlHandler, this, nodeSocket);
		tr.detach();
	}

	else
	{
		std::cout << "Client " << clientIPStr << ":" << clientPortStr << " is not allowed. Closing connection." << std::endl;
		closesocket(nodeSocket);
	}

}

void Server::clientHandler(const SOCKET client_socket)
{
    try
    {
		std::list<std::pair<std::string, std::string>> control_info;
		RequestInfo ri;
        std::string msg = ""; 
		RequestResult rr = RequestResult();
		TorRequestHandler torRequestHandler = TorRequestHandler(dm);
        char recvMsg[100];
        char buffer[128];
        std::string containerID;

        std::cout << "get msg from client " + std::to_string(client_socket) << std::endl; 

		ri = Helper::waitForResponse(client_socket);
		rr = torRequestHandler.directRequest(ri);
		unsigned int amountToOpen = DeserializerRequests::deserializeNodeOpeningRequest(ri.buffer).amount_to_open;
		dm.GetControlInfo(amountToOpen);
        Helper::sendVector(client_socket, rr.buffer);
        std::cout << "sending msg...\n";
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Server::clientControlHandler(const SOCKET node_sock)
{
	//make the logic of get alive msg 
	//only get them in the start thats it.

	try
	{
		//std::list<std::pair<std::string, std::string>> control_info;
		RequestInfo ri;
		std::string msg = "";
		RequestResult rr = RequestResult();
		TorRequestHandler torRequestHandler = TorRequestHandler(dm);
		char recvMsg[100];
		char buffer[128];
		std::string containerID;

		std::cout << "get msg from client " + std::to_string(node_sock) << std::endl;

		ri = Helper::waitForResponse(node_sock);
		rr = torRequestHandler.directRequest(ri);
		unsigned int amountToOpen = DeserializerRequests::deserializeNodeOpeningRequest(ri.buffer).amount_to_open;
		dm.GetControlInfo(amountToOpen);
		Helper::sendVector(node_sock, rr.buffer);
		std::cout << "sending msg...\n";
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
	}
}


int main()
{
    try
    {
        WSAInitializer wsa = WSAInitializer();
        Server server = Server(); 
        server.serve();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    system("pause");
    return 0; 
}