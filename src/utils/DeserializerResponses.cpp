#include "DeserializerResponses.h"

RsaKeyExchangeResponse DeserializerResponses::deserializeRsaKeyExchangeResponse(const std::vector<unsigned char>& buffer)
{
    RsaKeyExchangeResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data(), INIT_VEC_SIZE);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    
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

EcdheKeyExchangeResponse DeserializerResponses::deserializeEcdheKeyExchangeResponse(const std::vector<unsigned char>& buffer)
{
    EcdheKeyExchangeResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data(), INIT_VEC_SIZE);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.calculationResult = uint256_t{ std::string(jsonData["public_key"]) };   
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

CircuitConfirmationResponse DeserializerResponses::deserializeCircuitConfirmationResponse(const std::vector<unsigned char>& buffer)
{
    CircuitConfirmationResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data(), INIT_VEC_SIZE);

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);

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

HttpGetResponse DeserializerResponses::deserializeHttpGetResponse(const std::vector<unsigned char>& buffer)
{
    HttpGetResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data(), INIT_VEC_SIZE);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    

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