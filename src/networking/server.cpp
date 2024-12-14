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

	_controlList = std::map<unsigned int, std::list<std::pair<std::string, std::string>>>();

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
		TorRequestHandler torRequestHandler = TorRequestHandler(std::ref(dm), std::ref(this->_controlList));//you can delete and call to the handler from here
		mutex.unlock();
		char recvMsg[100];
		char buffer[128];
		std::string containerID;

		std::cout << "get msg from client " + std::to_string(client_socket) << std::endl;

		ri = Helper::waitForResponse(client_socket);
		rr = torRequestHandler.directRequest(ri);
		//can help -dm.GetControlInfo(amountToOpen); 
		// //here you get list of control settings to reach from server to node
		//in addition you want to get map<unsigned int, list<pair<string, string>>>
		// 
		// 	WE DONT DO THiS =>	//dont forget you need to call from here to serveControl only one time!!!
		// 
		//DONT FORGET HERE YOU ARE ONLY UPDATING THE LIST OF CIRCUIT THE CONTROL SERVER RUNS IN THE START
		Helper::sendVector(client_socket, rr.buffer);
		std::cout << "sending msg...\n";
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
	}
}



void Server::serveControl() //check if its one of the nodes
{
	try
	{
		bindAndListenControl();
		std::string input_string;
		std::list<string> clientsAlowde;
		while (true)
		{
			// the main thread is only accepting clients 
			// and add then to the list of handlers
			
			
			if (!_controlList.empty())
			{
				std::cout << "accepting node for control...\n";
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

void Server::acceptControlClient(const std::list<string>& allowedClients)
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

	// Check if the client is in the allowed list
	bool isAllowed = false;
	for (const auto& allowedClient : allowedClients) {
		if (allowedClient == clientIPStr) {
			isAllowed = true;
			break;
		}
	}

	if (isAllowed)
	{
		std::cout << "Node " << clientIPStr << " accepted." << std::endl;
		std::thread tr(&Server::clientControlHandler, this, nodeSocket);
		tr.detach();
	}
	else
	{
		std::cout << "Node " << clientIPStr << " is not allowed. Closing connection." << std::endl;
		closesocket(nodeSocket);
	}

}

void Server::clientControlHandler(const SOCKET node_sock)
{
	try
	{
		RequestInfo ri;
		char buffer[100];
		DWORD timeout = SECONDS_TO_WAIT * 1000;

		/*
		* GUVRIEL IT IS FOR U SEARCH WHAT THIS FUNCTION DO BC 
		* I DONT HAVE STRANGHT TO EXPLAIN 
		* AND IN LINE 255 YOU SEE TIMEOUT ERROR BY THAT YOU DONT EAVEN NEED A TIMER
		* BC setsockopt BUTIFUL FUNCTION!!!
		*/
		setsockopt(node_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		while (true)
		{
			int bytesRead = recv(node_sock, buffer, sizeof(buffer), 0);
			if (bytesRead <= 0 /* || buffer[0] == '\0'*/)
			{
				if (WSAGetLastError() == WSAETIMEDOUT)
				{
					std::cerr << "TIMEOUT: Node did not send alive message.\n";
					throw std::runtime_error("Timed out waiting for alive message.");
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