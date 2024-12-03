#include "RSA.h"

RSA::RSA()
{
	uint1024_t q = this->getRandomPrimeNumber<uint1024_t>(), p = this->getRandomPrimeNumber<uint1024_t>();
	this->N = calcProduct(q, p);
	this->T = calcTotient(q, p);
	selectPublicKey();
	selectPrivateKey();
	std::cout << "done making public and private keys!!!\n";
}

vector<unsigned char> RSA::Encrypt(vector<unsigned char>& text)
{
	// char ^ E MODE N
	vector<unsigned char> cypher_text;
	cpp_int textValue;
	for (auto it = text.begin(); it != text.end(); it++)
	{
		textValue += *it;
	}
	cpp_int cypherValue = mod_exp<uint2048_t>(textValue, this->E, this->N);
	size_t byteCount = (msb(cypherValue) / 8) + 1;
	for (size_t i = 0; i < byteCount; i++)
	{
		cypher_text.emplace_back(static_cast<unsigned char>((cypherValue >> (i * 8)) & 0xFF));
	}
	return cypher_text;
}

vector<unsigned char> RSA::Decrypt(vector<unsigned char>& cypher_text)
{
	// char ^ D MODE N
	vector<unsigned char> text;
	cpp_int cypherValue;
	for (auto it = text.begin(); it != text.end(); it++)
	{
		cypherValue += *it;
	}
	cpp_int textValue = mod_exp<uint2048_t>(cypherValue, this->D, this->N);
	size_t byteCount = (msb(textValue) / 8) + 1;
	for (size_t i = 0; i < byteCount; i++)
	{
		text.emplace_back(static_cast<unsigned char>((textValue >> (i * 8)) & 0xFF));
	}
	return text;
}
	
uint2048_t RSA::calcProduct(const uint1024_t& q, const uint1024_t& p)
{
	return static_cast<uint2048_t>(q * p);
}
	
uint2048_t RSA::calcTotient(const uint1024_t& q, const uint1024_t& p)
{
	return static_cast<uint2048_t>((q - 1) * (p - 1));
}
	
void RSA::selectPublicKey()
{
	while (true)
	{
		E = getRandomPrimeNumber<uint2048_t>();
		if (E >= T) 
			continue;
		if (!(T % E))
			continue;
		break;
	}
}
	
void RSA::selectPrivateKey()
{
	cpp_int mul_saver;
	while (true)
	{
		D = getRandomPrimeNumber<uint2048_t>(); 
		mul_saver = D * E;
		if (mul_saver % (cpp_int)(T) == 1)
			break;
	}
}
