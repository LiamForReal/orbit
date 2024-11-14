#include "DeserializerRequests.h"
//TODO: RSA & ECDHE deserialization
RsaKeyExchangeRequest DeserializerRequests::deserializeRsaKeyExchangeRequest(const std::vector<unsigned char>& buffer)
{

}
EcdheKeyExchangeRequest DeserializerRequests::deserializeEcdheKeyExchangeRequest(const std::vector<unsigned char>& buffer)
{

}

NodeOpenRequest DeserializerRequests::deserializeNodeOpeningRequest(const std::vector<unsigned char>& buffer)
{
	NodeOpenRequest request;
	std::string jsonDataStr = "";
	unsigned int len = 0, i = 0;

	std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
	std::cout << len << std::endl;
    
    for (i = 0; i < len; i++)
	{
		jsonDataStr += buffer[BYTES_TO_COPY + INC + i];
	}
    
	std::cout << jsonDataStr;
	json jsonData = json::parse(jsonDataStr);

	try
	{
		request.circuit_id = jsonData["circuit_id"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	return request;
}

LinkRequest DeserializerRequests::deserializeLinkRequest(const std::vector<unsigned char>& buffer)
{

}
HttpGetRequest DeserializerRequests::deserializeHttpGetRequest(const std::vector<unsigned char>& buffer)
{

}
CloseConnectionRequest DeserializerRequests::deserializeCloseConnectionRequest(const std::vector<unsigned char>& buffer)
{

}
DeleteCircuitRequest DeserializerRequests::deserializeDeleteCircuitRequest(const std::vector<unsigned char>& buffer)
{
    
}
GetDomainRequest DeserializerRequests::deserializeGetDomainRequest(const std::vector<unsigned char>& buffer)
{

}