#include "client.h"

#define MAX_NODES_TO_OPEN 5
#define MIN_NODES_TO_OPEN 0
static const unsigned short PORT = 9787;

Client::Client()
{
	// we connect to server that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_clientSocket == INVALID_SOCKET)
		throw std::runtime_error("server run error socket");

}

Client::~Client()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(_clientSocket);
	}
	catch (...) {}
}


void Client::connectToServer(std::string serverIP, int port)
{

	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(port); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr =  inet_addr(serverIP.c_str());    // the IP of the server

	// the process will not continue until the server accepts the client
	int status = connect(_clientSocket, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::runtime_error("Cant connect to server");
}

void Client::startConversation()
{
	int nodes_to_use = 0, nodes_to_open = 0; 
    char m[100];
    std::string msg = "";
	do
	{
		std::cout << "enter amount of nodes to open (between " + std::to_string(MIN_NODES_TO_OPEN) + " - " + std::to_string(MAX_NODES_TO_OPEN) +  "): ";
		std::cin >> nodes_to_open;
		std::cout << "enter amount of nodes to use: ";
		std::cin >> nodes_to_use;
	}while(nodes_to_open > MAX_NODES_TO_OPEN || nodes_to_open < MIN_NODES_TO_OPEN || nodes_to_use < MIN_NODES_TO_OPEN);
	msg = std::to_string(nodes_to_open) + " " + std::to_string(nodes_to_use);
	send(_clientSocket, msg.c_str(), msg.size(), 0);  // last parameter: flag. for us will be 0.
	std::cout << "Message send to server..." << std::endl;

	
	recv(_clientSocket, m, 99, 0);
    std::cout << "Message from server: " << m << std::endl;
}

int main()
{
	RsaKeyExchangeRequest r = {.public_key = uint1024_t("3829328329382392382398232983293823982392832329328392328329323982328932932828")};
	SerializerRequests::serializeRequest(r);

	return 0;

    try
    {
		WSAInitializer wsa = WSAInitializer();
		Client client = Client(); 
        client.connectToServer("127.0.0.1", COMMUNICATE_SERVER_PORT);
        client.startConversation();
    }
    catch(const std::runtime_error e)
    {
        std::cerr << e.what() << '\n';
    }
    system("pause");
    return 0; 
}
