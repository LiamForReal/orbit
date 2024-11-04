#pragma once 
#include <iostream> 
#include <boost/multiprecision/cpp_int.hpp>
using uint1024_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<1024, 1024, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>, boost::multiprecision::et_off>;
using std::pair; 
using std::string;

typedef enum RequestCode
{
	RSA_KEY_EXCHANG_RC = 10,
	ECDHE_KEY_EXCHANG_RC = 20,
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
    uint1024_t public_key;
} RsaKeyExchangeRequest;

typedef struct EcdheKeyExchangeRequest
{
	//base 
    //moduler
    //calc_result 
} EcdheKeyExchangeRequest;

typedef struct NodeOpenRequest
{
    unsigned int amout_to_open; 
    unsigned int amout_to_use; 
    unsigned int circuit_id; 
} NodeOpenRequest;

typedef struct LinkRequest
{
    pair<string, unsigned int> nextNode;
    unsigned int circuit_id; 
} LinkRequest;

typedef struct HttpGetRequest
{ 
    string msg; //maybe in cpp the librery have other way to save the content and it will be swiched there 
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


typedef struct AliveMsgRequest 
{
    unsigned char data;
} AliveMsgRequest;
