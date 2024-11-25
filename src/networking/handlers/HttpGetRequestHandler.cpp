#include "HttpGetRequestHandler.h"
#include <ws2tcpip.h>

HttpGetRequestHandler::HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData) : circuitsData(circuitsData)
{
	this->rr = RequestResult();
	this->circuitsData = circuitsData;
}

bool HttpGetRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == HTTP_MSG_RC;
}

std::string HttpGetRequestHandler::sendHttpRequest(const std::string& httpRequest)
{
   
}


        //std::string response = sendHttpGetRequest(domain);

RequestResult HttpGetRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	this->rr.buffer.clear();

	HttpGetRequest hgRequest;
	HttpGetResponse hgResponse;

	try
	{
		hgRequest = DeserializerRequests::deserializeHttpGetRequest(requestInfo.buffer);
		
        rr.circuit_id = hgRequest.circuit_id;
		// check if there is next
		if (this->circuitsData[hgRequest.circuit_id].second != INVALID_SOCKET)
		{
			hgResponse.status = HTTP_MSG_STATUS_FOWARD;
			std::vector<unsigned char> buffer = SerializerRequests::serializeRequest(hgRequest);
			Helper::sendVector(this->circuitsData[hgRequest.circuit_id].second, buffer);
		}
		else
		{
			// send HTTP GET (hgRequest.msg) to Web Server
			hgResponse.status = HTTP_MSG_STATUS_BACKWARD;
			hgResponse.content = this->sendHttpRequest(hgRequest.msg);
		}
	}
	catch (std::runtime_error e)
	{
		hgResponse.status = Errors::HTTP_MSG_ERROR;
		hgResponse.content = "";
	}

	this->rr.buffer = SerializerResponses::serializeResponse(hgResponse);
	return this->rr;
}
