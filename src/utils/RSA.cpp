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
	for (auto it = text.begin(); it != text.end(); it++)
	{
		cypher_text.emplace_back(mod_exp<uint2048_t, unsigned char>(*it, this->E, this->N));
	}
	return cypher_text;
}

vector<unsigned char> RSA::Decrypt(vector<unsigned char>& cypher_text)
{
	// char ^ D MODE N
	vector<unsigned char> text;
	for (auto it = cypher_text.begin(); it != cypher_text.end(); it++)
	{
		text.emplace_back(mod_exp<uint2048_t, unsigned char>(*it, this->D, this->N));
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
