#include "SerializerResponses.h"

std::vector<unsigned char> SerializerResponses::serializeResponse(const RsaKeyExchangeResponse& rsaKeyExchangeResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;
    json publicKey = rsaKeyExchangeResponse.public_key.str();
    json product = rsaKeyExchangeResponse.product.str();
    
    json requestJson = 
    {
        {"public_key", publicKey},
        {"product", product},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
    std::cout << "Length of data of key exchange: " << len << std::endl;
	std::memcpy(vec.data(), &len, INIT_VEC_SIZE);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const EcdheKeyExchangeResponse& ecdheKeyExchangeResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;

    json calcResult = ecdheKeyExchangeResponse.calculationResult.str();
    json requestJson = {
        {"calcResult", calcResult},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data(), &len, INIT_VEC_SIZE);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const CircuitConfirmationResponse& circuitConfirmationResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;

    json requestJson = {
        {"nodesPath", circuitConfirmationResponse.nodesPath},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data(), &len, INIT_VEC_SIZE);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const HttpGetResponse& httpGetResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;

    json requestJson = {
        {"content", httpGetResponse.content},
    };
    
    std::string requestJsonStr = requestJson.dump();
    std::cout << requestJsonStr << std::endl;
	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data(), &len, INIT_VEC_SIZE);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}