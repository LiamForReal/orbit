#include "server.h"

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
	_socket= socket(AF_INET,  SOCK_STREAM,  IPPROTO_TCP); 
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
	    std::list<std::string> nodesIp;
        char recvMsg[100];
        char buffer[128];
        std::string containerID;

        std::cout << "get msg from client " + std::to_string(client_socket) << std::endl; 
        int res = recv(client_socket, recvMsg, AMOUNT_OF_BYTES, 0);
        if (res == INVALID_SOCKET)
        {
            std::string s = "Error while receiving from socket: ";
            s += std::to_string(client_socket);
            throw std::runtime_error(s.c_str());
        }

		ri = waitForClientResponse(client_socket);

        std::cout << "client sent: " << ri.id << " , buffer: " << ri.buffer.data() << std::endl;  

		NodeOpenRequest nor = DeserializerRequests::deserializeNodeOpeningRequest(ri.buffer);

        // here open and get ips from docker.
        nodesIp = DockerManager::openAndGetIPs(nor.amount_to_use, nor.amount_to_open);

	    msg = "[";
	    for(auto it = nodesIp.begin(); it != nodesIp.end(); it ++)
	    {
		    msg += " " + *it + ",";
	    }
	    msg = msg.substr(0,msg.length() - 1);
	    msg += " ]";
        std::cout << "sending msg...\n";
        int bytesSent = send(client_socket, msg.c_str(), msg.length(), 0);
        if(bytesSent == -1)
            throw std::runtime_error("Failed to send");

		res = recv(client_socket, recvMsg, AMOUNT_OF_BYTES, 0);
        if (res == INVALID_SOCKET)
        {
            std::string s = "Error while receiving from socket: ";
            s += std::to_string(client_socket);
            throw std::runtime_error(s.c_str());
        }


		
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
}

RequestInfo Server::buildRI(SOCKET clientSocket, unsigned int statusCode)
{
    RequestInfo ri = RequestInfo();
    ri.buffer = std::vector<unsigned char>();
    std::string clientMsg = "";
    unsigned int clientMsgLength = 0;
	unsigned int circuitId = 0;
    size_t i = 0;
    int j = 0;

    ri.id = statusCode;

    std::cout << "DEBUG: Status code: " << statusCode << std::endl;
    ri.buffer.insert(ri.buffer.begin(), 1, static_cast<unsigned char>(statusCode));

    if (false) //request how has no data
        return ri;

	//ri.circuit_id = Helper::getCircuitIdFromSocket(clientSocket);

    //std::cout << "DEBUG: Circuit id: " << ri.circuit_id << std::endl;
    //ri.buffer.insert(ri.buffer.begin(), 1, static_cast<unsigned char>(ri.circuit_id));

    clientMsgLength = Helper::getLengthPartFromSocket(clientSocket);

    std::cout << "DEBUG: Length: " << clientMsgLength << std::endl;
    // Insert message length in little-endian format
    for (j = 0; j < BYTES_TO_COPY; ++j) {
        ri.buffer.insert(ri.buffer.begin() + INC + j, static_cast<unsigned char>((clientMsgLength >> (8 * j)) & 0xFF));
    }

    clientMsg = Helper::getStringPartFromSocket(clientSocket, clientMsgLength);
    clientMsg[clientMsgLength] = '\0';

    for (i = 0; i < clientMsgLength; i++)
    {
        ri.buffer.push_back(static_cast<unsigned char>(clientMsg[i]));
    }

    std::cout << "DEBUG: The message is: " << clientMsg << std::endl;

    ri.id = statusCode;

    return ri;
}

RequestInfo Server::waitForClientResponse(SOCKET socket)
{
	unsigned int statusCode; 
	while(true)
	{
		statusCode = Helper::socketHasData(socket);
		if(statusCode != 0 && statusCode != -1)
		{
			return buildRI(socket, statusCode);
		}
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