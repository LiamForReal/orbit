#include "SerializerRequests.h"

std::vector<unsigned char> SerializerRequests::serializeRequest(const RsaKeyExchangeRequest& rsaKeyExchangeRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;

    json public_key = rsaKeyExchangeRequest.public_key.str();
    json product = rsaKeyExchangeRequest.product.str();

    std::cout << public_key << std::endl;

    json requestJson = {
        {"public_key", public_key},
        {"product", product},
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

// TODO
std::vector<unsigned char> SerializerRequests::serializeRequest(const EcdheKeyExchangeRequest& ecdheKeyExchangeRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;

    json base = ecdheKeyExchangeRequest.b.str();
    json modular = ecdheKeyExchangeRequest.m.str();
    json calcResult = ecdheKeyExchangeRequest.calculationResult.str();

    std::cout << "base: " << base << "\nmodular: " << modular << "\ncalcResult: " << calcResult;
    json requestJson = {
        {"base", base},
        {"modular", modular},
        {"calc_result", calcResult},
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

std::vector<unsigned char> SerializerRequests::serializeRequest(const NodeOpenRequest& nodeOpenRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;

    json requestJson = {
        {"amount_to_open", nodeOpenRequest.amount_to_open},
        {"amount_to_use", nodeOpenRequest.amount_to_use},
    };
    
    std::string requestJsonStr = requestJson.dump();

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data(), &len, INIT_VEC_SIZE);
   
    vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const LinkRequest& linkRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;
    json requestJson = {
        {"nextNode", linkRequest.nextNode},
    };
    
    std::string requestJsonStr = requestJson.dump();

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data(), &len, INIT_VEC_SIZE);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const HttpGetRequest& httpGetRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);

    unsigned int len = 0;

    json requestJson = {
        {"domain", httpGetRequest.domain},
    };
    
    std::string requestJsonStr = requestJson.dump();

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data(), &len, INIT_VEC_SIZE);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}
