#include "client.h"
using std::vector;

Client::Client()
{
	// we connect to server that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocketWithDS = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_clientSocketWithFirstNode = INVALID_SOCKET;

	if (_clientSocketWithDS == INVALID_SOCKET)
		throw std::runtime_error("server run error socket");

}

Client::~Client()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(_clientSocketWithFirstNode);
		closesocket(_clientSocketWithDS);
	}
	catch (...) {}
}

void Client::connectToServer(std::string serverIP, int port)
{

	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(port); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());    // the IP of the server

	// the process will not continue until the server accepts the client
	int status = connect(_clientSocketWithDS, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::runtime_error("Cant connect to server");
}

void Client::nodeOpening()
{
	NodeOpenRequest nor;
	do
	{
		std::cout << "enter amount of nodes to open (between " + std::to_string(MIN_NODES_TO_OPEN) + " - " + std::to_string(MAX_NODES_TO_OPEN) + "): ";
		std::cin >> nor.amount_to_open;
		std::cout << "enter amount of nodes to use: ";
		std::cin >> nor.amount_to_use;
	} while (nor.amount_to_open > MAX_NODES_TO_OPEN || nor.amount_to_open < MIN_NODES_TO_OPEN || nor.amount_to_use < MIN_NODES_TO_OPEN);
	std::vector<unsigned char> data = SerializerRequests::serializeRequest(nor);
	Helper::sendVector(_clientSocketWithDS, data);
	std::cout << "Message send to server..." << std::endl;

	//CircuitConfirmationResponse ccr = DeserializerResponses::deserializeCircuitConfirmationResponse();
}

std::string Client::generateHttpGetRequest(const std::string& domain) 
{
	std::ostringstream request;
	//request << "GET / HTTP/1.1\r\n";
	//request << "Host: " << domain << "\r\n";
	//request << "Connection: close\r\n";
	//request << "\r\n";
	request << domain;
	return request.str();
}

bool Client::domainValidationCheck(std::string domain)
{
	//acomplish it
	return true;
}

void Client::startConversation()
{
	char buffer[100];
	std::string domain;
	RequestInfo ri;
	nodeOpening();
	ri = Helper::waitForResponse(this->_clientSocketWithDS);
	CircuitConfirmationResponse ccr = DeserializerResponses::deserializeCircuitConfirmationResponse(ri.buffer);

	for (auto it = ccr.nodesPath.begin(); it != ccr.nodesPath.end(); it++)
	{
		std::cout << "Node: " << it->first << " " << it->second << std::endl;
	}

	//headers of client sock begin
	_clientSocketWithFirstNode = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (_clientSocketWithFirstNode == INVALID_SOCKET)
		throw std::runtime_error("Client run error socket");

	int optval = 1;
	if (setsockopt(_clientSocketWithFirstNode, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) != 0) {
		perror("Failed to set socket option SO_REUSEADDR");
	}

	std::cout << "Connecting to " << ccr.nodesPath.begin()->first << " " << ccr.nodesPath.begin()->second << std::endl;

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(stoi(ccr.nodesPath.begin()->second));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(ccr.nodesPath.begin()->first.c_str());
	int status = connect(_clientSocketWithFirstNode, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
	{
		int errCode = WSAGetLastError();  // Get the error code from Winsock
		std::cerr << "Connect failed with error code: " << errCode << std::endl;
		throw std::runtime_error("Could not open socket with first node");
	}
	else std::cout << "connected successfully to the first node\n";
    //headers of client sock end
	if (ccr.nodesPath.size() > 1)
	{
		for (auto it = ccr.nodesPath.begin(); it != ccr.nodesPath.end(); it++)
		{
			if (it == ccr.nodesPath.begin())
				it++;
			LinkRequest linkRequest;
			linkRequest.nextNode = std::pair<std::string, unsigned int>(it->first, stoi(it->second));
			linkRequest.circuit_id = ccr.circuit_id;

			std::cout << "sended link msg\n";
			Helper::sendVector(_clientSocketWithFirstNode, SerializerRequests::serializeRequest(linkRequest));

			ri = Helper::waitForResponse(_clientSocketWithFirstNode);
			if (Errors::LINK_ERROR == ri.id)
			{
				throw std::runtime_error("Could not build circuit.");
			}
		}
	}
	
	while (true)
	{
		std::cout << "Enter domain: ";
		std::cin >> domain;
		if (!domainValidationCheck(domain))
			throw std::runtime_error("domain is illegal");

		HttpGetRequest httpGetRequest;
		httpGetRequest.circuit_id = ccr.circuit_id;
		httpGetRequest.domain = domain;

		std::cout << "sends httpGet Request:\n";
		Helper::sendVector(_clientSocketWithFirstNode, SerializerRequests::serializeRequest(httpGetRequest));

		ri = Helper::waitForResponse(_clientSocketWithFirstNode);

		HttpGetResponse httpGetResponse;
		httpGetResponse = DeserializerResponses::deserializeHttpGetResponse(ri.buffer);

		if (Errors::HTTP_MSG_ERROR == httpGetResponse.status)
		{
			std::cerr << "Could not get HTML of " << domain << std::endl;
		}
		else
		{
			std::cout << "HTML of " << domain << ": " << std::endl;
			std::cout << httpGetResponse.content << std::endl;
		}
	}
}

int main()
{
	try
	{
		WSAInitializer wsa = WSAInitializer();
		Client client = Client();
		client.connectToServer("127.0.0.1", COMMUNICATE_SERVER_PORT);
		client.startConversation();
	}
	catch (const std::runtime_error e)
	{
		std::cerr << e.what() << '\n';
	}
	system("pause");
	return 0;
}
