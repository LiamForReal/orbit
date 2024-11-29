#include "RSA.h"

RSA::RSA()
{
	uint2048_t q = getRandomPrimeNumber() ,p = getRandomPrimeNumber();
	this->P = calcProduct(q, p);
	this->T = calcTotient(q, p);
	selectPublicKey();
	selectPrivateKey();
	
}

vector<unsigned char> RSA::Encrypt()
{
	vector<unsigned char> vec;

	return vec;
}

vector<unsigned char> RSA::Decrypt()
{
	vector<unsigned char> vec;

	return vec;
}

uint2048_t RSA::getRandomPrimeNumber()
{
	return prime_numbers_manager::getRandomPrimeNumber();
}
	
cpp_int RSA::calcProduct(const cpp_int q, const cpp_int p)
{
	return q * p;
}
	
cpp_int RSA::calcTotient(const cpp_int q, const cpp_int p)
{
	return (q - 1) * (p - 1);
}
	
void RSA::selectPublicKey()
{
	this->public_key = uint2048_t();
	while (true)
	{
		this->public_key = getRandomPrimeNumber();
		if (this->public_key >= T)
			continue;
		if (!(T % this->public_key))
			continue;
		break;
	}
}
	
void RSA::selectPrivateKey()
{
	this->private_key = uint2048_t();
	while (true)
	{
		this->private_key = getRandomPrimeNumber();
		if ((private_key * public_key) % T == 1)
			break;
	}
}
