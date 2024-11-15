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
	LinkRequest request;
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
		request.nextNode = jsonData["nextNode"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return request;
}

HttpGetRequest DeserializerRequests::deserializeHttpGetRequest(const std::vector<unsigned char>& buffer)
{
	HttpGetRequest request;
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
		request.msg = jsonData["msg"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return request;
}

CloseConnectionRequest DeserializerRequests::deserializeCloseConnectionRequest(const std::vector<unsigned char>& buffer)
{
	CloseConnectionRequest request;
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

DeleteCircuitRequest DeserializerRequests::deserializeDeleteCircuitRequest(const std::vector<unsigned char>& buffer)
{
    DeleteCircuitRequest request;
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

GetDomainRequest DeserializerRequests::deserializeGetDomainRequest(const std::vector<unsigned char>& buffer)
{
	GetDomainRequest request;
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
		request.domain = jsonData["domain"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return request;
}