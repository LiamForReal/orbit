#include "Helper.h"


using std::string;

void Helper::sendVector(const SOCKET sc, const std::vector<uint8_t>& vec)
{
	try
	{
		int dataSize = vec.size(), bytesSent = 0, totalBytesSent = 0;
		if (vec.size() >= 2 && vec[1] != unsigned char(ALIVE_MSG_RC))
		{
			std::cout << "sending..." << "\n";
			std::cout << "status: " << unsigned int(vec[1]) << ", circuit id: " << unsigned int(vec[0]) << "\n";
			std::cout.flush();
			//std::cout << "only to show somting wrong\n";
			//std::cout << "Socket to send: " << sc << ", data size: " << dataSize << " bytes" << std::endl;
		}
			
		while (totalBytesSent < dataSize)
		{
			bytesSent = send(sc,
				reinterpret_cast<const char*>(vec.data()) + totalBytesSent, // Fix pointer arithmetic
				dataSize - totalBytesSent,
				0);

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
		if (vec.size() >= 2 && vec[1] != unsigned char(ALIVE_MSG_RC))
		{
			std::cout << "Successfully sent " << totalBytesSent << " bytes\n";
			std::cout.flush();
		}
			
	}
	catch (std::runtime_error& e)
	{
		std::cout << "run time error: " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "unecpected error while sending" << std::endl;
	}
}

unsigned int Helper::getStatusCodeFromSocket(const SOCKET sc)
{
	unsigned int value = 0;
	unsigned char* data = NULL;

	data = new unsigned char[1];
	recv(sc, (char*)(data), 1, 0);
	value = (unsigned int)(*data);

	delete[] data;
	data = NULL;

	return value;
}

unsigned int Helper::getCircuitIdFromSocket(const SOCKET sc)
{
	unsigned int value = 0;
	unsigned char* data = NULL;
	data = new unsigned char[1];
	recv(sc, (char*)(data), 1, 0);
	value = (unsigned int)(*data);

	delete[] data;
	data = NULL;

	return value;
}

unsigned int Helper::getLengthPartFromSocket(const SOCKET sc)
{
	int i = 0;
	unsigned int value = 0;
	std::vector<unsigned char> data = Helper::getDataPartFromSocket(sc, 4, 0); // Assuming getPartFromSocket returns the byte data as a string

	for (i = 0; i < 4; i++)
	{
		value |= (static_cast<unsigned int>(data[i]) << (i * 8));
	}

	return value;
}

// recieve data from socket according byteSize
// this is private function

std::vector<uint8_t> Helper::getDataPartFromSocket(const SOCKET sc, const int bytesNum, const int flags)
{
	if (bytesNum <= 0)
	{
		return {};
	}

	std::vector<uint8_t> data(bytesNum);
	int totalReceived = 0;

	while (totalReceived < bytesNum)
	{
		int res = recv(sc, reinterpret_cast<char*>(data.data()) + totalReceived,
			bytesNum - totalReceived, flags);

		if (res == SOCKET_ERROR)
		{
			std::cerr << "Error while receiving from socket: " << sc
				<< ", Error Code: " << WSAGetLastError() << std::endl;
			throw - 1;
		}
		else if (res == 0)
		{
			std::cerr << "Connection closed by the peer\n";
			throw - 1;//std::system_error(EBADF, std::generic_category(), "Connection closed by the peer");
		}

		totalReceived += res;
	}

	return data;
}


RequestInfo Helper::buildRI(const SOCKET& socket, const unsigned int& circuit_id, const unsigned int& statusCode)
{
	RequestInfo ri = RequestInfo();
	ri.buffer = std::vector<unsigned char>();
	size_t i = 0;
	int j = 0;

	ri.circuit_id = circuit_id;
	std::cout << "DEBUG: circuit id: " << ri.circuit_id << "\n";

	ri.id = statusCode;
	std::cout << "DEBUG: Status code: " << ri.id << std::endl;

	ri.length = Helper::getLengthPartFromSocket(socket);

	std::cout << "DEBUG: Length: " << ri.length << std::endl;

	ri.buffer = getDataPartFromSocket(socket, ri.length, 0);

	std::cout << "DEBUG: The message is: ";
	if (ri.buffer.size() < 2000)
	{
		for (auto it : ri.buffer)
			std::cout << it;
		std::cout << std::endl;
	}
	else std::cout << "the cipher text is roghly a 69000 bytes encripted by rsa2048 that I dont print.\n";
	return ri;
}


RequestInfo Helper::waitForResponse(const SOCKET& socket)
{
	RequestInfo ri;
	ri.circuit_id = Helper::getCircuitIdFromSocket(socket);
	ri.id = Helper::getStatusCodeFromSocket(socket);
	if (ri.id == ALIVE_MSG_RC || ri.id == CLOSE_CONNECTION_RC || ri.id == DELETE_CIRCUIT_RC
		|| ri.id == NODE_OPEN_STATUS || ri.id == LINK_STATUS || ri.id == CLOSE_CONNECTION_STATUS ||
		ri.id == ALIVE_MSG_STATUS || ri.id == DELETE_CIRCUIT_STATUS) //request how has no data
		return ri;
	else if (ri.circuit_id != 0 && ri.id == NODE_OPEN_RC)
	{
		ri.length = 0;
		return ri;
	}
	return Helper::buildRI(socket, ri.circuit_id, ri.id);
}

RequestInfo Helper::buildRI_RSA(const SOCKET& socket, const unsigned int& circuit_id, const unsigned int& statusCode, RSA& rsa)
{
	RequestInfo ri = RequestInfo();
	ri.buffer = std::vector<unsigned char>();
	unsigned int circuitId = 0;
	size_t i = 0;
	int j = 0;

	ri.circuit_id = circuit_id;
	std::cout << "DEBUG: circuit id: " << ri.circuit_id << "\n";

	ri.id = statusCode;
	std::cout << "DEBUG: Status code: " << ri.id << std::endl;

	ri.length = Helper::getLengthPartFromSocket(socket);
	std::cout << "DEBUG: Length: " << ri.length << std::endl;
	// msgLengthValue * 256 bytes because our RSA is 2048 bits and data's length is msgLengthValue
	std::vector<uint8_t> encryptedMessage;
	encryptedMessage.reserve(ri.length);
	encryptedMessage = getDataPartFromSocket(socket, ri.length, 0);

	//std::cout << "<===== FULL ENCRIPTED MSG START =====>\n";
	//for (auto it : encryptedMessageVec)
	//{
	//	std::cout << it;
	//}
	//std::cout << "\n<===== FULL ENCRIPTED MSG END =====>";
	std::cout << "DEBUG: The message is: ";
	try
	{
		std::vector<uint8_t> decryptedMessageVec = rsa.Decrypt(encryptedMessage);
		for (auto it : decryptedMessageVec)
		{
			ri.buffer.emplace_back(it);
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	return ri;
}

RequestInfo Helper::waitForResponse_RSA(const SOCKET& socket, RSA& rsa)
{
	RequestInfo ri;
	ri.circuit_id = Helper::getCircuitIdFromSocket(socket);
	ri.id = Helper::getStatusCodeFromSocket(socket);
	if (ri.id == ALIVE_MSG_RC || ri.id == CLOSE_CONNECTION_RC || ri.id == DELETE_CIRCUIT_RC
		|| ri.id == NODE_OPEN_STATUS || ri.id == LINK_STATUS || ri.id == CLOSE_CONNECTION_STATUS ||
		ri.id == ALIVE_MSG_STATUS || ri.id == DELETE_CIRCUIT_STATUS)//in case of regenerate circuit //request how has no data
		return ri;
	return Helper::buildRI_RSA(socket, ri.circuit_id, ri.id, std::ref(rsa));
}

RequestInfo Helper::buildRI_AES(const SOCKET& socket, const unsigned int& circuit_id, const unsigned int& statusCode, bool gotFromNext, AES& key)
{
	RequestInfo ri = RequestInfo();
	ri.buffer = std::vector<unsigned char>();
	size_t i = 0;
	int j = 0;

	ri.circuit_id = circuit_id;
	std::cout << "DEBUG: circuit id: " << ri.circuit_id << "\n";

	ri.id = statusCode;
	std::cout << "DEBUG: Status code: " << ri.id << std::endl;

	ri.length = Helper::getLengthPartFromSocket(socket);
	std::cout << "DEBUG: Length: " << ri.length << std::endl;

	ri.buffer = getDataPartFromSocket(socket, ri.length, 0);

	if (gotFromNext)
	{
		std::cout << "encripting...\n";
		ri.buffer = key.encrypt(ri.buffer);
	}
	else
	{
		std::cout << "decripting...\n";
		if (ri.buffer.size() < 2000)
		{
			std::cout << "cipher text: ";
			for (auto it : ri.buffer)
				std::cout << it;
			std::cout << "\n";
		}
		else std::cout << "the cipher text is roghly a 69000 bytes encripted by rsa2048 that I dont print.\n";
		
		ri.buffer = key.decrypt(ri.buffer);
	}
	std::cout << "DEBUG: The message is: " << ri.buffer.data() << std::endl;
	return ri;
}

RequestInfo Helper::waitForResponse_AES(const SOCKET& socket, AES& key, bool isEncription)
{
	RequestInfo ri;
	ri.circuit_id = Helper::getCircuitIdFromSocket(socket);
	ri.id = Helper::getStatusCodeFromSocket(socket);
	if (ri.id == ALIVE_MSG_RC || ri.id == CLOSE_CONNECTION_RC || ri.id == DELETE_CIRCUIT_RC
		|| ri.id == NODE_OPEN_STATUS || ri.id == LINK_STATUS || ri.id == CLOSE_CONNECTION_STATUS ||
		ri.id == ALIVE_MSG_STATUS || ri.id == DELETE_CIRCUIT_STATUS) //request how has no data
		return ri;
	return Helper::buildRI_AES(socket, ri.circuit_id, ri.id, isEncription, key);
}

vector<unsigned char> Helper::buildRR(RequestInfo& ri)
{
	vector<unsigned char> tmp;
	unsigned int len;

	len = ri.buffer.size();
	//std::cout << "msg len: " << len << ", ri.length: " << ri.length << "\n";
	ri.length = len; //when encripting sometimes rsa and esa add nulls padding

	//Insert Circuit id
	tmp.emplace_back(unsigned char(ri.circuit_id));

	//Insert Id
	tmp.emplace_back(unsigned char(ri.id));

	//Insert Length
	unsigned char* lengthBytes = reinterpret_cast<unsigned char*>(&len);
	tmp.insert(tmp.end(), lengthBytes, lengthBytes + 4);

	//Insert data
	if (!ri.buffer.empty())
		tmp.insert(tmp.end(), ri.buffer.begin(), ri.buffer.end());
	return tmp;
}

vector<unsigned char> Helper::buildRR(const vector<unsigned char> buffer, unsigned int status, unsigned int length, unsigned int circuit_id)
{
	vector<unsigned char> tmp;

	//Insert Circuit id
	tmp.emplace_back(unsigned char(circuit_id));

	//Insert ID
	tmp.emplace_back(unsigned char(status));

	//Insert length
	unsigned char* lengthBytes = reinterpret_cast<unsigned char*>(&length);
	tmp.insert(tmp.end(), lengthBytes, lengthBytes + 4);

	//Insert data
	tmp.insert(tmp.end(), buffer.begin(), buffer.end());
	return tmp;
}

vector<unsigned char> Helper::buildRR(unsigned int status, unsigned int circuit_id)
{
	vector<unsigned char> tmp;
	tmp.emplace_back(unsigned char(circuit_id));
	tmp.emplace_back(unsigned char(status));
	return tmp;
}
