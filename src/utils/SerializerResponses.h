#pragma once 

#include <iostream>
#include "Responses.hpp"
#include "json.hpp"

//server

class SerializerResponses
{
    public: 
        static std::vector<unsigned char> serializeResponse( const RsaKeyExchangeResponse& rsaKeyExchangeResponse);
        static std::vector<unsigned char> serializeResponse( const EcdheKeyExchangeResponse& ecdheKeyExchangeResponse);
        static std::vector<unsigned char> serializeResponse( const NodeOpenResponse& nodeOpenResponse);
        static std::vector<unsigned char> serializeResponse( const CircuitConfirmationResponse& circuitConfirmationResponse);
        static std::vector<unsigned char> serializeResponse( const BandwidthResponse& bandwidthResponse);
        static std::vector<unsigned char> serializeResponse( const LinkResponse& linkResponse);
        static std::vector<unsigned char> serializeResponse( const HttpGetResponse& httpGetResponse);
        static std::vector<unsigned char> serializeResponse( const CloseConnectionResponse& closeConnectionResponse);
        static std::vector<unsigned char> serializeResponse( const DeleteCircuitResponse& deleteCircuitResponse);
        static std::vector<unsigned char> serializeResponse( const AliveMsgResponse& aliveMsgResponse);

};

