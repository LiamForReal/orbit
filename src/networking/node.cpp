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

void Node::serveProxy()
{
	bindAndListen();
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
void Node::bindAndListen()
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
		std::string msg = "";
		
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
		WSAInitializer wsa = WSAInitializer();
		Node node = Node();
		node.serveProxy();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}
	system("pause");
	return 0;
}