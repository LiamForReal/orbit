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

std::string Node::getEnvVar(const LPCTSTR& key)
{ 
	char* container = new char[ENVE_MAX_LIMIT]; // Maximum size for environment variables on Windows
	LPTSTR buffer = (LPTSTR)(container);
	DWORD length = GetEnvironmentVariable(key, buffer, sizeof(buffer));
	if (length > 0 && length < sizeof(buffer)) 
	{
		string bufferStr = (char*)(buffer);
		delete container;
		return bufferStr;
	}
	throw std::runtime_error("ip not founded");
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
		Node node = Node();
		WSAInitializer wsa = WSAInitializer();
		string ip_env = node.getEnvVar((LPCTSTR)("NODE_IP")); // Get the IP from the environment variable
		string port_env = node.getEnvVar((LPCTSTR)("NODE_PORT")); // Get the port from the environment variable

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