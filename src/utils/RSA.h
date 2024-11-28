#pragma once 

#include "prime_numbers_manager.h"
#include <vector>
using std::vector;
class RSA
{
public: 
	RSA();
	vector<unsigned char> Encript();
	vector<unsigned char> Decript();


private: 
	cpp_int calcProduct(const cpp_int q, const cpp_int p);
	cpp_int calcTotient(const cpp_int q, const cpp_int p);
	void selectPublicKey();
	void selectPrivateKey(); 
	uint2048_t getRandomPrimeNumber();

	uint2048_t public_key;
	uint2048_t private_key;
	cpp_int T;
	cpp_int P;
};