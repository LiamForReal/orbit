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
	//BANDWIDTH_STATUS = 51,
	LINK_STATUS = 61,
	HTTP_MSG_STATUS = 71,
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
	uint2048_t public_key;
	uint2048_t product;
} RsaKeyExchangeResponse;

typedef struct EcdheKeyExchangeResponse
{
	uint256_t calculationResult;
} EcdheKeyExchangeResponse;

typedef struct CircuitConfirmationResponse
{
    std::vector<pair<string, string>> nodesPath; 
} CircuitConfirmationResponse;

typedef struct HttpGetResponse
{
	string content;  //html index.html file
} HttpGetResponse;