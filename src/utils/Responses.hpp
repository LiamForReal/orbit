#pragma once 
#include <iostream> 
#include <list> 
using std::list;
using std::pair; 
using std::string;
#include <boost/multiprecision/cpp_int.hpp>
using uint1024_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<1024, 1024, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>, boost::multiprecision::et_off>;

typedef enum Status
{
	RSA_KEY_EXCHANG_STATUS = 11,
	ECDHE_KEY_EXCHANG_STATUS = 21,
	NODE_OPEN_STATUS = 31,
	CIRCUIT_CONFIRMATION_STATUS = 41,
	BANDWIDTH_STATUS = 51,
	LINK_STATUS = 61,
	HTTP_MSG_STATUS = 71,
	CLOSE_CONNECTION_STATUS = 81,
	ALIVE_MSG_STATUS = 91,
	DELETE_CIRCUIT_STATUS = 101,

} Status;


typedef enum Errors
{
	RSA_KEY_EXCHANG_ERROR = 15,
	ECDHE_KEY_EXCHANG_ERROR = 25,
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
	uint1024_t public_key;
} RsaKeyExchangeResponse;

typedef struct EcdheKeyExchangeResponse
{
	unsigned int status; 
	//calc resulte
} EcdheKeyExchangeResponse;

typedef struct NodeOpenResponse
{
	unsigned int status; 
} NodeOpenResponse;

typedef struct CircuitConfirmationResponse
{
    list<pair<string, unsigned int>> nodesPath; 
} CircuitConfirmationResponse;

typedef struct BandwidthResponse
{
	unsigned int status; 
} BandwidthResponse;

typedef struct LinkResponse
{
	unsigned int status; 
} LinkResponse;

typedef struct HttpGetResponse
{
	unsigned int status;
	string content;  //html
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

