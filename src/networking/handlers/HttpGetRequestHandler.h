#pragma comment(lib, "Ws2_32.lib")
#include "IRequestHandler.h" 

class HttpGetRequestHandler : virtual public IRequestHandler
{
	public:
		HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData);
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		
		RequestResult handleRequest(const RequestInfo& requestInfo) override;
		

	private:

		RequestResult rr;
		std::string sendHttpRequest(const std::string& httpRequest);
		std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData;
};