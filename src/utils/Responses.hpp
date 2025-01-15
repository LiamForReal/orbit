#pragma once 
#include <iostream> 
#include <list> 
#include "utils.hpp"

using std::list;
using std::pair; 
using std::string;

typedef enum Status
{
	RSA_KEY_EXCHANGE_STATUS = 11,
	ECDHE_KEY_EXCHANGE_STATUS = 21,
	NODE_OPEN_STATUS = 31,
	CIRCUIT_CONFIRMATION_STATUS = 41,
	BANDWIDTH_STATUS = 51,
	LINK_STATUS = 61,
	HTTP_MSG_STATUS_FOWARD = 71,
	HTTP_MSG_STATUS_BACKWARD = 72,
	CLOSE_CONNECTION_STATUS = 81,
	ALIVE_MSG_STATUS = 91,
	DELETE_CIRCUIT_STATUS = 101,
} Status;


typedef enum Errors
{
	RSA_KEY_EXCHANGE_ERROR = 15,
	ECDHE_KEY_EXCHANGE_ERROR = 25,
	NODE_OPEN_ERROR = 35,
	CIRCUIT_CONFIRMATION_ERROR = 45,
	BANDWIDTH_ERROR = 55,
	LINK_ERROR = 65,
	HTTP_MSG_ERROR = 75,
	CLOSE_CONNECTION_ERROR = 85,
	ALIVE_MSG_ERROR = 95,
	DELETE_CIRCUIT_ERROR = 105,
} Errors;

typedef struct RsaKeyExchangeResponse
{
	unsigned int status; 
	uint2048_t public_key;
	uint2048_t product;
} RsaKeyExchangeResponse;

typedef struct EcdheKeyExchangeResponse
{
	unsigned int status; 
	uint256_t calculationResult;
} EcdheKeyExchangeResponse;

typedef struct NodeOpenResponse
{
	unsigned int status; 
} NodeOpenResponse;

typedef struct CircuitConfirmationResponse
{
	unsigned int status; 
    std::vector<pair<string, string>> nodesPath; 
} CircuitConfirmationResponse;

typedef struct LinkResponse
{
	unsigned int status; 
} LinkResponse;

typedef struct HttpGetResponse
{
	unsigned int status;
	string content;  //html index.html file
} HttpGetResponse;

typedef struct CloseConnectionResponse
{
	unsigned int status; 
} CloseConnectionResponse;

typedef struct AliveMsgResponse
{
	unsigned int status; 
} AliveMsgResponse;

typedef struct DeleteCircuitResponse
{
	unsigned int status; 
}  DeleteCircuitResponse;