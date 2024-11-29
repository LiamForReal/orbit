#include "client.h"
using std::vector;

Client::Client()
{
	// we connect to server that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocketWithDS = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_clientSocketWithFirstNode = NULL;

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

	_clientSocketWithFirstNode = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_clientSocketWithFirstNode == INVALID_SOCKET)
		throw std::runtime_error("Could not connect to first node");

	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(stoi(ccr.nodesPath.begin()->second));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(ccr.nodesPath.begin()->first.c_str());

	int status = connect(_clientSocketWithFirstNode, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::runtime_error("Could not open socket with first node");

	for (auto it = ccr.nodesPath.begin().operator++(); it != ccr.nodesPath.end(); it++)
	{
		LinkRequest linkRequest;
		linkRequest.nextNode = std::pair<std::string, unsigned int>(it->first, stoi(it->second));
		linkRequest.circuit_id = ccr.circuit_id;

		Helper::sendVector(_clientSocketWithFirstNode, SerializerRequests::serializeRequest(linkRequest));

		ri = Helper::waitForResponse(_clientSocketWithFirstNode);
		if (Errors::LINK_ERROR == ri.id)
		{
			throw std::runtime_error("Could not build circuit.");
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
		httpGetRequest.msg = "GET " + domain;

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
			std::cout << "HTML of " << domain << std::endl;
			std::cout << httpGetResponse.content << std::endl;
		}
	}
}

void test()
{
	uint2048_t b = uint2048_t("9114053668641702653451613590119705862579522968896220661891406825213816989086740065550445579597346168312826739478311195636977209590440509899626891805771158212618430992704586813367050694666985330130949656225117193233423522566028729729247554597051345987449274691819621444662743089456907740795333448841375469608959921848172910927602753023477215389392687077363469686574251550151461857418780337451611741578130099682778256085524594553144483029409428078835399945975597032992467238732116522467809190849963545776789396838736486213267743784160839562749047477151861383359409297673927652305370575713505184898991395485312127979590");
	uint2048_t p = uint2048_t("637986339873861326105107312255838253191438879880434473803457501216741677413864575381273995503867531438940895411908193918168993920821570993276560621751843923217401601643905040546830458063958179042187474218785450539576842659380807752444684861620718293161030057018808875666172271398308704384355625423990531546608449619403815461393806908433109177674885323803628637991704076681795180467205260980457671666508553252384393181053026812370592254722139774063443429543445791696672974322525438662056186979881513973924555080924995070158510975159680074836267156053498134650251915346433069810060088137197898758670975129847957100372");
	uint2048_t m = uint2048_t("976595587549532060209833036580025754611594781085891964623734229544205812137324496181590353269566870499316475998028523671383122619056457229080037226038865646226959625259318022320494934134472703501468266121570211589217350055323332895532715713062304177097302093607996575853663755110722156492871317870479535617741404396753461192001683365854059285628001222578012566803853851080922670595545085969707148502426746992643274259783947736614664495491303998739489411670730645018472125529373641395955808559913038348892181505118923482608406538317399175428373013176266626431136866272470882379485481307332375823843606001274604945515");

	std::cout << "(" << b << " ^ " << p << ") % " << m << " = " << mod_exp<uint2048_t>(b, p, m) << std::endl;
}

int main()
{
	test();
	return 0;

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
