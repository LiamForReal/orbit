#include "DeserializerResponses.h"

// TODO: fix
RsaKeyExchangeResponse DeserializerResponses::deserializeRsaKeyExchangeResponse(const std::vector<unsigned char>& buffer)
{
    RsaKeyExchangeResponse response;
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
		response.status = jsonData["status"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;
}

// TODO: add more stuff maybe
EcdheKeyExchangeResponse DeserializerResponses::deserializeEcdheKeyExchangeResponse(const std::vector<unsigned char>& buffer)
{
    EcdheKeyExchangeResponse response;
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
		response.status = jsonData["status"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;
}

NodeOpenResponse DeserializerResponses::deserializeNodeOpeningResponse(const std::vector<unsigned char>& buffer)
{
    NodeOpenResponse response;
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
		response.status = jsonData["status"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;
}

LinkResponse DeserializerResponses::deserializeLinkResponse(const std::vector<unsigned char>& buffer)
{
    LinkResponse response;
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
		response.status = jsonData["status"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;
}

HttpGetResponse DeserializerResponses::deserializeHttpGetResponse(const std::vector<unsigned char>& buffer)
{
    HttpGetResponse response;
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
		response.status = jsonData["status"];
        response.content = jsonData["content"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;
}

CloseConnectionResponse DeserializerResponses::deserializeCloseConnectionResponse(const std::vector<unsigned char>& buffer)
{
    CloseConnectionResponse response;
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
		response.status = jsonData["status"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;
}

DeleteCircuitResponse DeserializerResponses::deserializeDeleteCircuitResponse(const std::vector<unsigned char>& buffer)
{
    DeleteCircuitResponse response;
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
		response.status = jsonData["status"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;    
}

GetDomainResponse DeserializerResponses::deserializeGetDomainResponse(const std::vector<unsigned char>& buffer)
{
    GetDomainResponse response;
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
		response.status = jsonData["status"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}
	
	return response;
}
