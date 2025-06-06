#pragma once

#include <vector>
#include <string>
#include <WinSock2.h>
#include <chrono>
#include "Requestes.hpp"
#include "Responses.hpp"
#include "RequestInfo.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "AES.h"
#include "RSA.h"

#define INC 1
#define BYTES_TO_COPY 4


class Helper
{
public:
	static void sendVector(const SOCKET sc, const std::vector<unsigned char>& vec);

	static unsigned int getStatusCodeFromSocket(const SOCKET sc); 
	static unsigned int getCircuitIdFromSocket(const SOCKET sc); 
	static unsigned int getLengthPartFromSocket(const SOCKET sc); 
	static std::vector<unsigned char> getDataPartFromSocket(const SOCKET sc, const int bytesNum, const int flags);

	static RequestInfo buildRI(const SOCKET& socket, const unsigned int& circuit_id, const unsigned int& statusCode);
	static RequestInfo waitForResponse(const SOCKET& socket);

	static RequestInfo buildRI_RSA(const SOCKET& socket, const unsigned int& circuit_id, const unsigned int& statusCode, RSA& rsa);
	static RequestInfo waitForResponse_RSA(const SOCKET& socket, RSA& rsa);

	static RequestInfo buildRI_AES(const SOCKET& socket, const unsigned int& circuit_id, const unsigned int& statusCode, bool gotFromNext,AES& key);
	static RequestInfo waitForResponse_AES(const SOCKET& socket, AES& key, bool isEncription);

	static vector<unsigned char> buildRR(RequestInfo& ri);
	static vector<unsigned char> buildRR(const vector<unsigned char> buffer, unsigned int status, unsigned int length, unsigned int circuit_id = 0);
	static vector<unsigned char> buildRR(unsigned int status, unsigned int circuit_id = 0);
};