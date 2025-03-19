#include "SerializerResponses.h"

std::vector<unsigned char> SerializerResponses::serializeResponse(const RsaKeyExchangeResponse& rsaKeyExchangeResponse)
{
    std::vector<unsigned char> vec;

    json publicKey = rsaKeyExchangeResponse.public_key.str();
    json product = rsaKeyExchangeResponse.product.str();
    
    json requestJson = 
    {
        {"public_key", publicKey},
        {"product", product},
    };
    
    std::string requestJsonStr = requestJson.dump();

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const EcdheKeyExchangeResponse& ecdheKeyExchangeResponse)
{
    std::vector<unsigned char> vec;

    json calcResult = ecdheKeyExchangeResponse.calculationResult.str();
    json requestJson = {
        {"calcResult", calcResult},
    };
    
    std::string requestJsonStr = requestJson.dump();

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const CircuitConfirmationResponse& circuitConfirmationResponse)
{
    std::vector<unsigned char> vec;

    json requestJson = {
        {"nodesPath", circuitConfirmationResponse.nodesPath},
    };
    
    std::string requestJsonStr = requestJson.dump();

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const HttpGetResponse& httpGetResponse)
{
    std::vector<unsigned char> vec;

    json requestJson = {
        {"content", httpGetResponse.content},
    };
    
    std::string requestJsonStr = requestJson.dump();
    std::cout << requestJsonStr << std::endl;

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}