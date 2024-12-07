#include "DeserializerRequests.h"

RsaKeyExchangeRequest DeserializerRequests::deserializeRsaKeyExchangeRequest(const std::vector<unsigned char>& buffer) 
{
    RsaKeyExchangeRequest request;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    std::cout << "Deserialized length: " << len << std::endl;

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    std::cout << "Deserialized JSON: " << jsonDataStr << std::endl;

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        request.public_key = uint2048_t{ std::string(jsonData["public_key"]) };
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return request;
}

EcdheKeyExchangeRequest DeserializerRequests::deserializeEcdheKeyExchangeRequest(const std::vector<unsigned char>& buffer)
{
	EcdheKeyExchangeRequest request;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    std::cout << "Deserialized length: " << len << std::endl;

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    std::cout << "Deserialized JSON: " << jsonDataStr << std::endl;

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		request.b = uint256_t{ std::string(jsonData["base"]) };
		request.m = uint256_t{ std::string(jsonData["modular"]) };
        request.calculationResult = uint256_t{ std::string(jsonData["calc_result"]) };
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return request;
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
    
	
	json jsonData = json::parse(jsonDataStr);

	try
	{
		request.amount_to_open = jsonData["amount_to_open"];
		request.amount_to_use = jsonData["amount_to_use"];
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

    for (i = 0; i < len; i++)
	{
		jsonDataStr += buffer[BYTES_TO_COPY + INC + i];
	}
    
	
	json jsonData = json::parse(jsonDataStr);

	try
	{
		request.circuit_id = jsonData["circuit_id"];
		request.domain = jsonData["domain"];
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