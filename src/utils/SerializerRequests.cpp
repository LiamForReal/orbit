#include "SerializerRequests.h"

std::vector<unsigned char> SerializerRequests::serializeRequest(const RsaKeyExchangeRequest& rsaKeyExchangeRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(RSA_KEY_EXCHANGE_RC));

    std::cout << "Public Key: " << rsaKeyExchangeRequest.public_key.str() << std::endl;

    json requestJson = {
        {"public_key", rsaKeyExchangeRequest.public_key.str()},
    };

    std::string requestJsonStr = requestJson.dump();
    std::cout << "Serialized JSON: " << requestJsonStr << std::endl;

    // Insert the length of the JSON message into the vector (4 bytes for length)
    unsigned int len = (unsigned int)requestJsonStr.size();
    std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

    // Resize the vector to hold the message
    vec.resize(INIT_VEC_SIZE + len);

    // Insert the serialized JSON string into the vector
    std::copy(requestJsonStr.begin(), requestJsonStr.end(), vec.begin() + INIT_VEC_SIZE);

    return vec;
}

// TODO
std::vector<unsigned char> SerializerRequests::serializeRequest(const EcdheKeyExchangeRequest& ecdheKeyExchangeRequest)
{
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const NodeOpenRequest& nodeOpenRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(NODE_OPEN_RC));

    unsigned int len = 0;

    json requestJson = {
        {"amount_to_open", nodeOpenRequest.amount_to_open},
        {"amount_to_use", nodeOpenRequest.amount_to_use},

        {"circuit_id", nodeOpenRequest.circuit_id},
    };
    
    std::string requestJsonStr = requestJson.dump();
	std::cout << requestJsonStr << std::endl;

	// Insert Message Length Into Vector
	len = (unsigned int)(requestJsonStr.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	vec.resize(INIT_VEC_SIZE + len);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

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

	vec.resize(INIT_VEC_SIZE + len);

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

	vec.resize(INIT_VEC_SIZE + len);

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

	vec.resize(INIT_VEC_SIZE + len);

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

	vec.resize(INIT_VEC_SIZE + len);

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

	vec.resize(INIT_VEC_SIZE + len);

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

	vec.resize(INIT_VEC_SIZE + len);

	// Insert Message Into Vector
	vec.insert(vec.end(), requestJsonStr.begin(), requestJsonStr.end());

    return vec;
}