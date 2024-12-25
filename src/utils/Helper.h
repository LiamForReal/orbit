#pragma once

#include <vector>
#include <string>
#include <WinSock2.h>
#include "Requestes.hpp"
#include "RequestInfo.hpp"
#include <chrono>
#define INC 1
#define BYTES_TO_COPY 4


class Helper
{
public:

	static unsigned int socketHasData(SOCKET socket);
	static std::string getStringPartFromSocket(SOCKET sc, const int bytesNum);
	static void sendVector(const SOCKET sc, const std::vector<unsigned char>& vec);
	//static void sendData(const SOCKET sc, const std::string message);
	static unsigned int getStatusCodeFromSocket(const SOCKET sc); //
    static unsigned int getCircuitIdFromSocket(const SOCKET sc); //
	static unsigned int getLengthPartFromSocket(const SOCKET sc); //
	static unsigned char* getUnsignedCharPartFromSocket(const SOCKET sc, const int bytesNum, const int flags);
	static RequestInfo buildRI(SOCKET socket, unsigned int statusCode);
	static RequestInfo waitForResponse(SOCKET socket,int timeout = -1);

private:
	static std::string getPartFromSocket(const SOCKET sc, const int bytesNum);
	static std::string getPartFromSocket(const SOCKET sc, const int bytesNum, const int flags);
};