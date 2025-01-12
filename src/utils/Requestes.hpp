#pragma once 
#include "utils.hpp"

using std::pair; 
using std::string;

typedef enum RequestCode
{
	RSA_KEY_EXCHANGE_RC = 10,
	ECDHE_KEY_EXCHANGE_RC = 20,
	NODE_OPEN_RC = 30,
	CIRCUIT_CONFIRMATION_RC = 40,     
	BANDWIDTH_RC = 50,
	LINK_RC = 60,    
	HTTP_MSG_RC = 70,
	CLOSE_CONNECTION_RC = 80, 
	ALIVE_MSG_RC = 90,
	DELETE_CIRCUIT_RC = 100,
} RequestCode;

typedef struct RsaKeyExchangeRequest
{
    uint2048_t public_key;
    uint2048_t product;
    unsigned int circuit_id;
} RsaKeyExchangeRequest;

typedef struct EcdheKeyExchangeRequest
{
    uint256_t b; 
    uint256_t m; 
    uint256_t calculationResult; 
    unsigned int circuit_id;
} EcdheKeyExchangeRequest;

typedef struct NodeOpenRequest
{
    unsigned int amount_to_open; 
    unsigned int amount_to_use; 
} NodeOpenRequest;

typedef struct LinkRequest
{
    pair<std::string, unsigned int> nextNode;
    unsigned int circuit_id; 
} LinkRequest;

typedef struct HttpGetRequest
{
    string domain; //maybe in cpp the library have other way to save the content and it will be switched there 
    unsigned int circuit_id;
} HttpGetRequest;

typedef struct CloseConnectionRequest
{ 
    unsigned int circuit_id; 
} CloseConnectionRequest;

typedef struct DeleteCircuitRequest
{ 
    unsigned int circuit_id; 
} DeleteCircuitRequest;