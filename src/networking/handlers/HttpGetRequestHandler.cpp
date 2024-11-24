#include "HttpGetRequestHandler.h"

HttpGetRequestHandler::HttpGetRequestHandler(std::map<unsigned int, std::pair<SOCKET, SOCKET>>& circuitsData) : circuitsData(circuitsData)
{
	this->rr = RequestResult();
	this->circuitsData = circuitsData;
}

bool HttpGetRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == HTTP_MSG_RC;
}

RequestResult HttpGetRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	this->rr.buffer.clear();

	HttpGetRequest hgRequest;
	HttpGetResponse hgResponse;

	try
	{
		hgRequest = DeserializerRequests::deserializeHttpGetRequest(requestInfo.buffer);

		// check if there is next
		if (this->circuitsData[hgRequest.circuit_id].second)
		{
			HttpGetRequest transferredRequest;

			// send hgRequest.msg to next Node
			hgResponse.content = "";

			transferredRequest.circuit_id = hgRequest.circuit_id;
			transferredRequest.msg = hgRequest.msg;

			std::vector<unsigned char> buffer = SerializerRequests::serializeRequest(transferredRequest);
			Helper::sendVector(this->circuitsData[hgRequest.circuit_id].second, buffer);
		}
		else
		{
			// send HTTP GET (hgRequest.msg) to Web Server

			hgResponse.content = "THIS SHOULD BE HTML!!!";
		}

		hgResponse.status = Status::HTTP_MSG_STATUS;
	}
	catch (std::runtime_error e)
	{
		hgResponse.status = Errors::HTTP_MSG_ERROR;
		hgResponse.content = "";
	}

	this->rr.buffer = SerializerResponses::serializeResponse(hgResponse);
	return this->rr;
}
