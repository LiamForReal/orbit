#include "SerializerRequests.h"

std::vector<unsigned char> SerializerRequests::serializeRequest(const RsaKeyExchangeRequest& rsaKeyExchangeRequest)
{
    std::vector<unsigned char> vec(INIT_VEC_SIZE);
    vec[0] = ((unsigned char)(RSA_KEY_EXCHANGE_RC));

    unsigned int len = 0;

    //json requestJson = {
    //    {"public_key", json(rsaKeyExchangeRequest.public_key)},
    //};

    // std::string requestJson = erJson.dump();
	//std::cout << requestJson << std::endl;

	// Insert Message Length Into Vector
	//len = (unsigned int)(requestJson.size()); // possible lose of data for 64 bits.
	std::memcpy(vec.data() + INC, &len, BYTES_TO_COPY);

	vec.resize(INIT_VEC_SIZE + len);

	// Insert Message Into Vector
	//vec.insert(vec.end(), requestJson.begin(), requestJson.end());


    return vec;
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const EcdheKeyExchangeRequest& ecdheKeyExchangeRequest)
{
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const NodeOpenRequest& nodeOpenRequest)
{
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const LinkRequest& linkRequest)
{
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const HttpGetRequest& httpGetRequest)
{
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const CloseConnectionRequest& closeConnectionRequest)
{
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const DeleteCircuitRequest& deleteCircuitRequest)
{
}

std::vector<unsigned char> SerializerRequests::serializeRequest(const AliveMsgRequest& aliveMsgRequest)
{
}