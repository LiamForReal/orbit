#pragma once

#include <vector>
#include <string>
#include <WinSock2.h>


class Helper
{
public:

	static unsigned int socketHasData(SOCKET socket);
	static int getMessageTypeCode(const SOCKET sc);
	static std::string getStringPartFromSocket(SOCKET sc, const int bytesNum);
	static void sendVector(const SOCKET sc, const std::vector<unsigned char>& vec);
	//static void sendData(const SOCKET sc, const std::string message);
	static unsigned int getStatusCodeFromSocket(const SOCKET sc); //
    static unsigned int getCircuitIdFromSocket(const SOCKET sc); //
	static unsigned int getLengthPartFromSocket(const SOCKET sc); //
	static unsigned char* getUnsignedCharPartFromSocket(const SOCKET sc, const int bytesNum, const int flags);

private:
	static std::string getPartFromSocket(const SOCKET sc, const int bytesNum);
	static std::string getPartFromSocket(const SOCKET sc, const int bytesNum, const int flags);
};