#include "node.h"
// add request handler that muches 

#define ENVE_MAX_LIMIT  32,766
#define AMOUNT_OF_BYTES 1250
// using static const instead of macros 
static const unsigned int IFACE = 0;

using std::string;
using std::vector;


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
				Helper::sendVector(this->circuits[rr.circuit_id].first, rr.buffer);
			}
			if (this->circuits[rr.circuit_id].second != INVALID_SOCKET && this->circuits[rr.circuit_id].second != NULL)
			{
				ri = Helper::waitForResponse(this->circuits[rr.circuit_id].second);
				rr = nodeRequestHandler.directMsg(ri); //change sock first add this methods
				//and change client_socket in node request handler to _socket
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

		node.serveProxy(ip_env, port);
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}

	system("pause");

	return 0;
}