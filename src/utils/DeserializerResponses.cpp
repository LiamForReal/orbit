#include "DeserializerResponses.h"

RsaKeyExchangeResponse DeserializerResponses::deserializeRsaKeyExchangeResponse(const RequestInfo ri)
{
    RsaKeyExchangeResponse response;


    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(ri.buffer.begin(), ri.buffer.end());
    
    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
        response.public_key = uint2048_t{ std::string(jsonData["public_key"]) };
        response.product = uint2048_t{ std::string(jsonData["product"]) };
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

EcdheKeyExchangeResponse DeserializerResponses::deserializeEcdheKeyExchangeResponse(const RequestInfo ri)
{
    EcdheKeyExchangeResponse response;

 
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(ri.buffer.begin(), ri.buffer.end());
    
    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try 
    {
        response.calculationResult = uint256_t{ std::string(jsonData["calcResult"]) };   
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

CircuitConfirmationResponse DeserializerResponses::deserializeCircuitConfirmationResponse(const RequestInfo ri)
{
    CircuitConfirmationResponse response;


    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(ri.buffer.begin(), ri.buffer.end());

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.nodesPath = jsonData["nodesPath"];
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

HttpGetResponse DeserializerResponses::deserializeHttpGetResponse(const RequestInfo ri)
{
    HttpGetResponse response;

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(ri.buffer.begin(), ri.buffer.end());

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.content = jsonData["content"];
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}