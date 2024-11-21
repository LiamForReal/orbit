#include "SerializerRequests.h"

std::vector<unsigned char> SerializerRequests::serializeRequest(const RsaKeyExchangeRequest& rsaKeyExchangeRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(RSA_KEY_EXCHANGE_RC));

    unsigned int len = 0;

    json public_key = rsaKeyExchangeRequest.public_key.str();

    std::cout << public_key << std::endl;

    json requestJson = {
        {"public_key", public_key},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

// TODO
std::vector<unsigned char> SerializerRequests::serializeRequest(const EcdheKeyExchangeRequest& ecdheKeyExchangeRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(ECDHE_KEY_EXCHANGE_RC));

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
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const NodeOpenRequest& nodeOpenRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(NODE_OPEN_RC));

    unsigned int len = 0;

    json requestJson = {
        {"amount_to_open", nodeOpenRequest.amount_to_open},
        {"amount_to_use", nodeOpenRequest.amount_to_use},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << "status code: " << NODE_OPEN_RC << "length: " << requestJsonStr.length() << "Data: " << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);
   
    vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());
	
    for (unsigned char c : vec) {
        printf("%02x ", c); // Print each byte as a two-digit hexadecimal
    }   

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const LinkRequest& linkRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(LINK_RC));

    unsigned int len = 0;

    json requestJson = {
        {"circuit_id", linkRequest.circuit_id},
        {"nextNode", linkRequest.nextNode},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const HttpGetRequest& httpGetRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(HTTP_MSG_RC));

    unsigned int len = 0;

    json requestJson = {
        {"circuit_id", httpGetRequest.circuit_id},
        {"msg", httpGetRequest.msg},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const CloseConnectionRequest& closeConnectionRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(CLOSE_CONNECTION_RC));

    unsigned int len = 0;

    json requestJson = {
        {"circuit_id", closeConnectionRequest.circuit_id},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const DeleteCircuitRequest& deleteCircuitRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(DELETE_CIRCUIT_RC));

    unsigned int len = 0;

    json requestJson = {
        {"circuit_id", deleteCircuitRequest.circuit_id},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const AliveMsgRequest& aliveMsgRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(ALIVE_MSG_RC));

    unsigned int len = 0;

    json requestJson = {
        {"data", aliveMsgRequest.data},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const GetDomainRequest& getDomainRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(GET_DOMAIN_RC));

    unsigned int len = 0;

    json requestJson = {
        {"domain", getDomainRequest.domain},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}