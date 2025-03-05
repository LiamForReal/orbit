#include "DeserializerRequests.h"

RsaKeyExchangeRequest DeserializerRequests::deserializeRsaKeyExchangeRequest(const RequestInfo ri) // unsigned int circuit_id 
{
    RsaKeyExchangeRequest request;
    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(ri.buffer.begin(), ri.buffer.end());

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try 
	{
		
        request.public_key = uint2048_t { std::string(jsonData["public_key"]) };
		request.product = uint2048_t { std::string(jsonData["product"]) };
    }
    catch (...) 
	{
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return request;
}

EcdheKeyExchangeRequest DeserializerRequests::deserializeEcdheKeyExchangeRequest(const RequestInfo ri)
{
	EcdheKeyExchangeRequest request;

    // Convert the serialized JSON string from the buffer
	std::string jsonDataStr(ri.buffer.begin(), ri.buffer.end());
	std::cout << "data is: " << jsonDataStr << "\n";
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

NodeOpenRequest DeserializerRequests::deserializeNodeOpeningRequest(const RequestInfo ri)
{
	NodeOpenRequest request;
	std::string jsonDataStr = "";
	unsigned int i = 0;
    
    for (i = 0; i < ri.length; i++)
	{
		jsonDataStr += ri.buffer[i];
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

LinkRequest DeserializerRequests::deserializeLinkRequest(const RequestInfo ri)
{
	LinkRequest request; 
	std::string jsonDataStr = "";
	unsigned int i = 0;
    
    for (i = 0; i < ri.length; i++)
	{
		jsonDataStr += ri.buffer[i];
	}
    
	
	json jsonData = json::parse(jsonDataStr);

	try
	{
		request.nextNode = jsonData["nextNode"];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid json structure passed");
	}

	return request;
}
HttpGetRequest DeserializerRequests::deserializeHttpGetRequest(const RequestInfo ri)
{
	HttpGetRequest request;
	std::string jsonDataStr = "";
	unsigned int i = 0;

    for (i = 0; i < ri.length; i++)
	{
		jsonDataStr += ri.buffer[i];
	}
    
	
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

//no need in close connection