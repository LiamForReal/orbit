#include "server.h"
#include "handlers/TorRequestHandler.h"

#define AMOUNT_OF_BYTES 1250
// using static const instead of macros 
static const unsigned short PORT = 9787;
static const unsigned int IFACE = 0;

using std::string;
using std::vector;


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


// listen to connecting requests from clients
// accept them, and create thread for each client
void Server::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
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
		RequestInfo ri;
        std::string msg = ""; 
		DockerManager dm = DockerManager();
		RequestResult rr = RequestResult();
		TorRequestHandler torRequestHandler = TorRequestHandler(dm);
        char recvMsg[100];
        char buffer[128];
        std::string containerID;

        std::cout << "get msg from client " + std::to_string(client_socket) << std::endl; 

		ri = Helper::waitForResponse(client_socket);
		rr = torRequestHandler.directRequest(ri);
        Helper::sendVector(client_socket, rr.buffer);
        std::cout << "sending msg...\n";
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
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