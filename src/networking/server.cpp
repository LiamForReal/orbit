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
    std::string msg = "";
	std::list<std::string> nodesIp;
    int nodes_to_open = 0, nodes_to_use = 0, space_pos = 0;
    char recvMsg[100]; 
    std::cout << "get msg from client " + std::to_string(client_socket) << std::endl; 
    int res = recv(client_socket, recvMsg, AMOUNT_OF_BYTES, 0);
    if (res == INVALID_SOCKET)
    {
        std::string s = "Error while receiving from socket: ";
        s += std::to_string(client_socket);
        throw std::runtime_error(s.c_str());
    }
    std::cout << "client sent: " << recvMsg << std::endl; 

    msg = recvMsg;
    space_pos = msg.find(' ');
    if(space_pos == -1 || space_pos == 0)
    {
        throw std::runtime_error("msg is illegal");
    }

    nodes_to_open = std::stoi(msg.substr(0, space_pos)); 
    nodes_to_use = std::stoi(msg.substr(space_pos + 1)); 

    std::string command = "docker-compose up --build -d";  // -d flag runs it in detached mode
    for (int i = 0; i < nodes_to_open; i++)
    {
        int result = std::system(command.c_str());
        if (result == 0) 
            std::cout << "Docker Compose process started successfully." << std::endl;
        else 
            std::cerr << "Failed to start Docker Compose process. Error code: " << result << std::endl;

        // Retrieve container ID and IP address
        std::string containerIDCommand = "docker-compose ps -q";
        FILE* pipe = _popen(containerIDCommand.c_str(), "r");
        if (!pipe) throw std::runtime_error("Failed to run command");

        char buffer[128];
        std::string containerID;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            containerID += buffer;
        }
        _pclose(pipe);

        containerID = containerID.substr(0, containerID.find("\n"));  // Clean up newlines

        std::string inspectCommand = "docker inspect -f \"{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}\" " + containerID;
        pipe = _popen(inspectCommand.c_str(), "r");
        if (!pipe) throw std::runtime_error("Failed to run inspect command");

        std::string containerIP;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            containerIP += buffer;
        }
        _pclose(pipe);
		nodesIp.push_back(containerIP);
    }

	msg = "[";
	for(auto it = nodesIp.begin(); it != nodesIp.end(); it ++)
	{
		msg += " " + *it + ",";
	}
	msg += " ]";
    std::cout << "sending msg...\n";
    int bytesSent = send(client_socket, msg.c_str(), msg.length(), 0);
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