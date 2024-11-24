#include "node.h"
// add request handler that muches 

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
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(0); //to change!!! pass as a ev
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
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
				Helper::sendVector(this->circuits[rr.circuit_id].first, rr.buffer);
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
		const char* ip_env = getenv("NODE_IP"); // Get the IP from the environment variable
		const char* port_env = getenv("NODE_PORT"); // Get the port from the environment variable

		const char* ip = ip_env ? ip_env : "0.0.0.0"; // Default to 0.0.0.0 if not set
		int port = port_env ? std::atoi(port_env) : 9050; // Default to 9050 if not set
		WSAInitializer wsa = WSAInitializer();
		Node node = Node();
		string ipStr = ip_env;
		uint16_t portUint = (uint16_t)(port_env);
		node.serveProxy(ipStr, portUint);
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}
	system("pause");
	return 0;
}