#include "SerializerResponses.h"

std::vector<unsigned char> SerializerResponses::serializeResponse(const RsaKeyExchangeResponse& rsaKeyExchangeResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(rsaKeyExchangeResponse.status));

    unsigned int len = 0;
    json publicKey = rsaKeyExchangeResponse.public_key.str();
    json product = rsaKeyExchangeResponse.product.str();
    std::cout << "public key: " << publicKey << std::endl;
    
    json requestJson = 
    {
        {"status", rsaKeyExchangeResponse.status},
        {"public_key", publicKey},
        {"product", product},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
    std::cout << "Length of data of key exchange: " << len << std::endl;
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const EcdheKeyExchangeResponse& ecdheKeyExchangeResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(ecdheKeyExchangeResponse.status));

    unsigned int len = 0;

    json calcResult = ecdheKeyExchangeResponse.calculationResult.str();
    json requestJson = {
        {"status", ecdheKeyExchangeResponse.status},
        {"calcResult", calcResult},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const NodeOpenResponse& nodeOpenResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(nodeOpenResponse.status));

    unsigned int len = 0;

    json requestJson = {
        {"status", nodeOpenResponse.status},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const CircuitConfirmationResponse& circuitConfirmationResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(circuitConfirmationResponse.status));

    unsigned int len = 0;

    json requestJson = {
        {"status", circuitConfirmationResponse.status},
        {"circuit_id", circuitConfirmationResponse.circuit_id},
        {"nodesPath", circuitConfirmationResponse.nodesPath},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const LinkResponse& linkResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(linkResponse.status));

    unsigned int len = 0;

    json requestJson = {
        {"status", linkResponse.status},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const HttpGetResponse& httpGetResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(httpGetResponse.status));

    unsigned int len = 0;

    json requestJson = {
        {"status", httpGetResponse.status},
        {"content", httpGetResponse.content},
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

std::vector<unsigned char> SerializerResponses::serializeResponse(const CloseConnectionResponse& closeConnectionResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(closeConnectionResponse.status));

    unsigned int len = 0;

    json requestJson = {
        {"status", closeConnectionResponse.status},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const DeleteCircuitResponse& deleteCircuitResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(deleteCircuitResponse.status));

    unsigned int len = 0;

    json requestJson = {
        {"status", deleteCircuitResponse.status},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}

std::vector<unsigned char> SerializerResponses::serializeResponse(const AliveMsgResponse& aliveMsgResponse)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(aliveMsgResponse.status));

    unsigned int len = 0;

    json requestJson = {
        {"status", aliveMsgResponse.status},
    };
    
    std::string requestJsonStr = requestJson.dump();
	

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}