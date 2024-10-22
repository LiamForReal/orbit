#include "client.h"



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
    char m[100];
    std::string msg = "";

	std::cout << "enter msg : " << std::endl;
	std::cin >> msg;
	send(_clientSocket, msg.c_str(), msg.size(), 0);  // last parameter: flag. for us will be 0.
	std::cout << "Message send to server..." << std::endl;

	
	recv(_clientSocket, m, 99, 0);
    std::cout << "Message from server: " << m << std::endl;
}

int main()
{
   
    try
    {
		WSAInitializer wsa = WSAInitializer();
		Client client = Client(); 
        client.connectToServer("127.0.0.1", PORT);
        client.startConversation();
    }
    catch(const std::runtime_error e)
    {
        std::cerr << e.what() << '\n';
    }
    system("pause");
    return 0; 
}
