#include "node.h"
// add request handler that muches 

#define ENVE_MAX_LIMIT  32,766
#define AMOUNT_OF_BYTES 1250
// using static const instead of macros 
static const unsigned int IFACE = 0;

using std::string;
using std::vector;
std::mutex mtx;

Node::Node()
{
	// notice that we step out to the global namespace
	// for the resolution of the function socket
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
		throw std::runtime_error("Node run error socket");
}

Node::~Node()
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

SOCKET Node::createSocketWithServer()
{
	SOCKET sock = INVALID_SOCKET;

	// Create the socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		throw std::runtime_error("Error creating socket: " + std::to_string(WSAGetLastError()));
	}

	// Resolve the server's IP address using `host.docker.internal`
	addrinfo hints = {};
	hints.ai_family = AF_INET;      // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP

	// Set up the sockaddr_in structure
	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(COMMUNICATE_CONTROL_NODE_PORT); // Port from your config 
	//serverAddr.sin_addr = reinterpret_cast<sockaddr_in*>(result->ai_addr)->sin_addr;
	if (inet_pton(AF_INET, "192.168.2.1", &serverAddr.sin_addr) <= 0) {
		closesocket(sock);
		WSACleanup();
		throw std::runtime_error("inet_pton failed: " + std::to_string(WSAGetLastError()));
	}

	//freeaddrinfo(result); // Free the addrinfo structure

	// Connect to the server
	if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(sock);
		throw std::runtime_error("Connection failed: " + std::to_string(WSAGetLastError()));
	}

	return sock;
}

void Node::serveControl()
{
	try
	{
		char* data = new char[1];
		RequestInfo ri;
		RequestResult rr;
		data[0] = (char)(ALIVE_MSG_RC);
		SOCKET serverSock = createSocketWithServer();
		int bytesSent; 

		NodeRequestHandler nodeRequestHandler = NodeRequestHandler(std::ref(circuits), serverSock);

		while (true)
		{
			mtx.lock();
			bytesSent = send(serverSock, data, sizeof(data), 0);
			mtx.unlock();
			if (bytesSent <= 0)
			{
				std::cout << "\n\n\n alive msg wasn't send \n\n\n";
				std::cout << "send: data: " << data << " , size of data: " << sizeof(data) << "\n";
				break;
			}
			//return; node crush
			// add node crush exe to check  
			mtx.lock();
			ri = Helper::waitForResponse(serverSock, 1);
			mtx.unlock();
			if (ri.buffer.empty())
				continue;
			
			rr = nodeRequestHandler.directMsg(ri);

			if (DELETE_CIRCUIT_STATUS == rr.buffer[0])
			{
				std::cout << "Delete successfully done!\n";
			}
			else
			{
				std::cerr << "Failed to delete circuit!\n";
			}
		}
	}
	catch (std::runtime_error& e)
	{
		std::cout << "control manganon problem\n";
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "an unaccespted error\n";
	}
}

void Node::serveProxy(const std::string& ip, uint16_t port)
{
	bindAndListen(ip, port);
	std::string input_string;
	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "accepting client...\n";
		acceptClient();
	}
}

// listen to connecting requests from clients
// accept them, and create thread for each client
void Node::bindAndListen(const std::string& ip, uint16_t port)
{
	std::cout << "NODE IP: " << ip << " NODE PORT: " << port << std::endl;

	int optval = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) < 0) {
		std::cerr << "Failed to set SO_REUSEADDR!" << std::endl;
		throw std::runtime_error("Failed to set SO_REUSEADDR");
	}

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(port);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	// again stepping out to the global namespace
	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::runtime_error("Node bind error");
	std::cout << "binded\n";

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error("Node listen error");
	std::cout << "listening...\n";

}

void Node::acceptClient()
{
	SOCKET client_socket = accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::runtime_error("invalid socket accepted");

	std::cout << "Client accepted !\n";
	// create new thread for client	and detach from it
	std::thread tr(&Node::clientHandler, this, client_socket);
	tr.detach();

}

std::string Node::getEnvVar(const LPCSTR& key) 
{
	// Buffer to hold the environment variable valu
	char buffer[(int)(ENVE_MAX_LIMIT)];

	// Get the environment variable
	DWORD length = GetEnvironmentVariableA(key, buffer, (DWORD)(ENVE_MAX_LIMIT));

	// Check the result
	if (length > 0 && length < ENVE_MAX_LIMIT) {
		return std::string(buffer); // Return the value as a string
	}
	else if (length == 0) {
		throw std::runtime_error("Environment variable not found: ");
	}
	else {
		throw std::runtime_error("Environment variable value is too large: ");
	}
}


void Node::clientHandler(const SOCKET client_socket)
{
	try
	{
		RequestInfo ri;
		LinkRequest lr;
		RequestResult rr;
		rr.circuit_id = 0;
		NodeRequestHandler nodeRequestHandler = NodeRequestHandler(std::ref(circuits), client_socket);
		while (true)
		{
			ri = Helper::waitForResponse(client_socket);
			rr = nodeRequestHandler.directMsg(ri);
			if ((unsigned int)(rr.buffer[0]) == LINK_STATUS || (unsigned int)(rr.buffer[0]) == HTTP_MSG_STATUS_BACKWARD)
			{
				std::cout << "sending beckward!\n";
				Helper::sendVector(circuits[rr.circuit_id].first, rr.buffer);
			}

			if ((unsigned int)(rr.buffer[0]) == HTTP_MSG_STATUS_FOWARD)
			{
				std::cout << "listening foward\n";
				ri = Helper::waitForResponse(this->circuits[rr.circuit_id].second);
				Helper::sendVector(this->circuits[rr.circuit_id].first, ri.buffer);
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}
}


int main()
{
	try
	{
		//change you have ct version go get it
		WSAInitializer wsa = WSAInitializer();
		Node node = Node();
		string ip_env = node.getEnvVar((LPCSTR)("NODE_IP")); // Get the IP from the environment variable
		string port_env = node.getEnvVar((LPCSTR)("NODE_PORT")); // Get the port from the environment variable
		std::cout << "ip is: " << ip_env << ", port is: " << port_env << "\n";
 		uint16_t port = (uint16_t)(std::atoi(port_env.c_str())); // Default to 9050 if not set

		std::thread aliveMsg(&Node::serveControl, node);
		aliveMsg.detach();
		node.serveProxy(ip_env, port);
	}
	catch (const std::runtime_error& e)
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