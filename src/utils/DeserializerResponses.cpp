#include "DeserializerResponses.h"

RsaKeyExchangeResponse DeserializerResponses::deserializeRsaKeyExchangeResponse(const std::vector<unsigned char>& buffer)
{
    RsaKeyExchangeResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    
    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		response.status = jsonData["status"];
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
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.status = jsonData["status"];
        response.calculationResult = uint256_t{ std::string(jsonData["public_key"]) };   
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

NodeOpenResponse DeserializerResponses::deserializeNodeOpeningResponse(const std::vector<unsigned char>& buffer)
{
    NodeOpenResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    
    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.status = jsonData["status"];
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
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.status = jsonData["status"];
        response.nodesPath = jsonData["nodesPath"];
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

LinkResponse DeserializerResponses::deserializeLinkResponse(const std::vector<unsigned char>& buffer)
{
    LinkResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.status = jsonData["status"];   
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
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.status = jsonData["status"];
        response.content = jsonData["content"];
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

CloseConnectionResponse DeserializerResponses::deserializeCloseConnectionResponse(const std::vector<unsigned char>& buffer)
{
    CloseConnectionResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.status = jsonData["status"];
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}

DeleteCircuitResponse DeserializerResponses::deserializeDeleteCircuitResponse(const std::vector<unsigned char>& buffer)
{
    DeleteCircuitResponse response;

    unsigned int len = 0;
    std::memcpy(&len, buffer.data() + INC, BYTES_TO_COPY);
    

    // Convert the serialized JSON string from the buffer
    std::string jsonDataStr(buffer.begin() + INIT_VEC_SIZE, buffer.begin() + INIT_VEC_SIZE + len);
    

    // Parse the JSON string
    json jsonData = json::parse(jsonDataStr);
    try {
		
        response.status = jsonData["status"];
    }
    catch (...) {
        throw std::runtime_error("Invalid JSON structure passed");
    }

    return response;
}