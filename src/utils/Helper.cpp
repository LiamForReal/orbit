#include "Helper.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

using std::string;

// recieves the type code of the message from socket (3 bytes)
// and returns the code. if no message found in the socket returns 0 (which means the client disconnected)

unsigned int Helper::socketHasData(SOCKET socket) 
{
	char buf;
	unsigned int code = Helper::getStatusCodeFromSocket(socket);
	return code;
}

// recieve data from socket according byteSize
// returns the data as string
string Helper::getStringPartFromSocket(const SOCKET sc, const int bytesNum)
{
	return getPartFromSocket(sc, bytesNum*sizeof(unsigned char), 0);
}

void Helper::sendVector(const SOCKET sc, const std::vector<unsigned char>& vec)
{
	std::cout << "Sending...\n";
	const char* dataPtr = reinterpret_cast<const char*>(vec.data());
	std::string str = dataPtr; 
	int dataSize = static_cast<int>(vec.size());
	int totalBytesSent = 0;
	std::cout << "socket to send: " << sc << std::endl;
	while (totalBytesSent < dataSize)
	{
		
		int bytesSent = send(sc, dataPtr + totalBytesSent, dataSize - totalBytesSent, 0);

		if (bytesSent == SOCKET_ERROR)
		{
			std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
			throw std::runtime_error("Error while sending message to client");
		}
		else if (bytesSent == 0)
		{
			std::cerr << "Connection closed by the client\n";
			throw std::runtime_error("Connection closed by the client");
		}

		totalBytesSent += bytesSent;
	}

	if (totalBytesSent != dataSize)
	{
		std::cerr << "Failed to send entire message to client\n";
		throw std::runtime_error("Failed to send entire message to client");
	}
}

unsigned int Helper::getStatusCodeFromSocket(const SOCKET sc)
{
	unsigned int value = 0;
	unsigned char* data = getUnsignedCharPartFromSocket(sc, 1, 0);

	value = (value << 8) | static_cast<unsigned char>(data[0]);

	delete[] data;
	data = NULL;

	return value;
}

unsigned int Helper::getCircuitIdFromSocket(const SOCKET sc)
{
    unsigned int value = 0;
	unsigned char* data = getUnsignedCharPartFromSocket(sc, 1, 0);

	value = (value << 8) | static_cast<unsigned char>(data[0]);

	delete[] data;
	data = NULL;

	return value;
}

unsigned int Helper::getLengthPartFromSocket(const SOCKET sc)
{
	int i = 0;
	unsigned int value = 0;
	unsigned char* data = Helper::getUnsignedCharPartFromSocket(sc, 4, 0); // Assuming getPartFromSocket returns the byte data as a string


	for (i = 0; i < 4; i++)
	{
		value |= (static_cast<unsigned int>(data[i]) << (i * 8));
	}

	delete[] data;
	data = NULL;

	return value;
}

// recieve data from socket according byteSize
// this is private function
std::string Helper::getPartFromSocket(const SOCKET sc, const int bytesNum)
{
	return getPartFromSocket(sc, bytesNum, 0);
}

// send data to socket
// this is private function
/*
    void Helper::sendData(const SOCKET sc, const std::string message)
{
	const char* data = message.c_str();

	if (send(sc, data, message.size(), 0) == INVALID_SOCKET)
	{
		throw std::exception("Error while sending message to client");
	}
}

*/


std::string Helper::getPartFromSocket(const SOCKET sc, const int bytesNum, const int flags)
{
	if (bytesNum == 0)
	{
		return "";
	}

	char* data = new char[bytesNum + 1];
	int res = recv(sc, data, bytesNum, flags);
	if (res == INVALID_SOCKET)
	{
		std::string s = "Error while recieving from socket: ";
		s += std::to_string(sc);
		throw std::runtime_error(s.c_str());
	}
	data[bytesNum] = 0;
	std::string received(data);
	delete[] data;
	data = NULL;

	return received;
}

unsigned char* Helper::getUnsignedCharPartFromSocket(const SOCKET sc, const int bytesNum, const int flags)
{
	if (bytesNum == 0)
	{
		return nullptr;
	}

	unsigned char* data = new unsigned char[bytesNum];
	int res = recv(sc, (char*)(data), bytesNum, flags);

	if (res == SOCKET_ERROR)
	{
		std::string s = "Error while receiving from socket: ";
		s += std::to_string(sc);
		throw std::runtime_error(s);
	}
	else if (res != bytesNum)
	{
		// Handle incomplete data reception if needed
	}

	//data[bytesNum] = '\0';

	return data;
}

RequestInfo Helper::buildRI(SOCKET socket, unsigned int statusCode)
{
    RequestInfo ri = RequestInfo();
    ri.buffer = std::vector<unsigned char>();
    std::string msg = "";
    unsigned int msgLength = 0;
	unsigned int circuitId = 0;
    size_t i = 0;
    int j = 0;

    ri.id = statusCode;

    std::cout << "DEBUG: Status code: " << statusCode << std::endl;
    ri.buffer.insert(ri.buffer.begin(), 1, static_cast<unsigned char>(statusCode));

    if (ri.id == ALIVE_MSG_RC) //request how has no data
        return ri;

    msgLength = Helper::getLengthPartFromSocket(socket);
    std::cout << "DEBUG: Length: " << msgLength << std::endl;

    for (j = 0; j < BYTES_TO_COPY; ++j) {
        ri.buffer.insert(ri.buffer.begin() + INC + j, static_cast<unsigned char>((msgLength >> (8 * j)) & 0xFF));
    }

    msg = Helper::getStringPartFromSocket(socket, msgLength);
    msg[msgLength] = '\0';

    for (i = 0; i < msgLength; i++)
    {
        ri.buffer.push_back(static_cast<unsigned char>(msg[i]));
    }

    std::cout << "DEBUG: The message is: " << msg << std::endl;

    ri.id = statusCode;

    return ri;
}


RequestInfo Helper::waitForResponse(SOCKET socket)
{
	unsigned int statusCode;

	while (true)
	{
		statusCode = Helper::socketHasData(socket);
		if (statusCode != 0 && statusCode != -1)
		{
			return Helper::buildRI(socket, statusCode);
		}
	}
}

RequestInfo Helper::buildRI_RSA(SOCKET socket, const unsigned int& statusCode, RSA& rsa)
{
	RequestInfo ri = RequestInfo();
	ri.buffer = std::vector<unsigned char>();
	std::string msg = "";
	unsigned int circuitId = 0;
	size_t i = 0;
	int j = 0;

	std::vector<uint8_t> encryptedLengthVec;
	// 4 * 256 bytes because our RSA is 2048 bits and length is 4
	encryptedLengthVec.reserve(4 * 256);
	unsigned int msgLengthValue = 0;

	unsigned char* encryptedLength = getUnsignedCharPartFromSocket(socket, 4 * 256, 0);
	for (short i = 0; i < 4 * 256; i++)
	{
		encryptedLengthVec.emplace_back(encryptedLength[i]);
	}

	free(encryptedLength);
	encryptedLength = NULL;

	std::vector<uint8_t> decryptedLengthVec = rsa.Decrypt(std::ref(encryptedLengthVec));

	for (i = 0; i < 4; i++)
	{
		msgLengthValue |= (static_cast<unsigned int>(decryptedLengthVec[i]) << (i * 8));
	}

	encryptedLengthVec.clear();
	decryptedLengthVec.clear();

	ri.id = statusCode;

	std::cout << "DEBUG: Status code: " << statusCode << std::endl;
	ri.buffer.insert(ri.buffer.begin(), 1, static_cast<unsigned char>(statusCode));

	if (ri.id == ALIVE_MSG_RC) //request how has no data
		return ri;

	std::cout << "DEBUG: Length: " << msgLengthValue << std::endl;

	for (j = 0; j < BYTES_TO_COPY; ++j) 
	{
		ri.buffer.insert(ri.buffer.begin() + INC + j, static_cast<unsigned char>((msgLengthValue >> (8 * j)) & 0xFF));
	}

	// msgLengthValue * 256 bytes because our RSA is 2048 bits and data's length is msgLengthValue
	std::vector<uint8_t> encryptedMessageVec;
	encryptedMessageVec.resize(msgLengthValue * 256);

	unsigned char* encryptedMessage = getUnsignedCharPartFromSocket(socket, msgLengthValue * 256, 0);
	for (unsigned int i = 0; i < msgLengthValue * 256; i++)
	{
		encryptedMessageVec.emplace_back(encryptedMessage[i]);
	}

	free(encryptedMessage);
	encryptedMessage = NULL;

	std::vector<uint8_t> decryptedMessageVec = rsa.Decrypt(std::ref(encryptedMessageVec));

	for (uint8_t byte : decryptedMessageVec)
	{
		msg += byte;
	}

	encryptedMessageVec.clear();
	decryptedMessageVec.clear();

	msg[msgLengthValue] = '\0';

	for (i = 0; i < msgLengthValue; i++)
	{
		ri.buffer.push_back(static_cast<unsigned char>(msg[i]));
	}

	std::cout << "DEBUG: The message is: " << msg << std::endl;

	ri.id = statusCode;

	return ri;
}

RequestInfo Helper::waitForResponse_RSA(SOCKET socket, RSA& rsa)
{
	std::vector<uint8_t> encryptedStatusCodeVec;
	// 256 bytes because our RSA is 2048 bits
	encryptedStatusCodeVec.reserve(256);
	unsigned int statusCode;

	unsigned char* encryptedStatusCode = getUnsignedCharPartFromSocket(socket, 256, 0);
	for (short i = 0; i < 256; i++)
	{
		encryptedStatusCodeVec.emplace_back(encryptedStatusCode[i]);
	}

	free(encryptedStatusCode);
	encryptedStatusCode = NULL;

	std::vector<uint8_t> decryptedStatusCodeVec = rsa.Decrypt(std::ref(encryptedStatusCodeVec));

	statusCode = decryptedStatusCodeVec[0];

	encryptedStatusCodeVec.clear();
	decryptedStatusCodeVec.clear();

	return Helper::buildRI_RSA(socket, statusCode, rsa);
}
