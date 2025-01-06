#pragma comment(lib, "Ws2_32.lib")
#include "IRequestHandler.h" 

typedef struct CurlResponse
{
	char* string;
	size_t size;
} CurlResponse;

class HttpGetRequestHandler : virtual public IRequestHandler
{
	public:
		HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData, SOCKET& clientSock);
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		
		RequestResult handleRequest(const RequestInfo& requestInfo) override;
		

	private:

		RequestResult rr;
		static size_t writeChunk(void* data, size_t size, size_t nmemb, void* userData);
		std::string sendHttpRequest(const std::string& httpRequest);
		std::map<unsigned int, std::pair<SOCKET, SOCKET>>& _circuitsData;
		SOCKET& _socket;
};