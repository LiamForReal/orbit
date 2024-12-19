#include "RSA.h"

RSA::RSA()
{
	this->_generatedP = false;
	this->_generatedQ = false;

	std::promise<uint1024_t> promiseQ;
	std::promise<uint1024_t> promiseP;

	std::future<uint1024_t> futurePromiseQ = promiseQ.get_future();
	std::future<uint1024_t> futurePromiseP = promiseP.get_future();

	std::thread generateQThread(&RSA::generateQ, this, std::move(promiseQ));
	std::thread generatePThread(&RSA::generateP, this, std::move(promiseP));

	generateQThread.join();
	generatePThread.join();

	while (!this->_generatedP || !this->_generatedQ)
	{

	}

	uint1024_t q = futurePromiseQ.get();
	uint1024_t p = futurePromiseP.get();

	std::cout << "q = " << q << std::endl << "p = " << p << std::endl;
	std::cout << "p and q generated successfully\n";

	this->N = calcProduct(q, p);
	std::cout << "N (product) = " << this->N << std::endl;
	this->T = calcTotient(q, p);
	std::cout << "T (toshient) = " << this->T << std::endl;
	selectPublicKey();
	std::cout << "E (public key) = " << this->E << std::endl;
	selectPrivateKey();
	std::cout << "D (public key) = " << this->D << std::endl;
	std::cout << "done making public and private keys!!!\n";
}

void RSA::Encrypt(vector<unsigned char>& text)
{
	// char ^ E MODE N
	cpp_int textValue;
	for (auto it = text.begin(); it != text.end(); it++)
	{
		textValue += *it;
	}
	text.clear();
	cpp_int cypherValue = mod_exp<uint2048_t>(textValue, this->E, this->N);
	size_t byteCount = (msb(cypherValue) / 8) + 1;
	for (size_t i = 0; i < byteCount; i++)
	{
		text.emplace_back(static_cast<unsigned char>((cypherValue >> (i * 8)) & 0xFF));
	}
}

void RSA::Decrypt(vector<unsigned char>& cypher_text)
{
	// char ^ D MODE Ns
	cpp_int cypherValue;
	for (auto it = cypher_text.begin(); it != cypher_text.end(); it++)
	{
		cypherValue += *it;
	}
	cypher_text.clear();
	cpp_int textValue = mod_exp<uint2048_t>(cypherValue, this->D, this->N);
	size_t byteCount = (msb(textValue) / 8) + 1;
	for (size_t i = 0; i < byteCount; i++)
	{
		cypher_text.emplace_back(static_cast<unsigned char>((textValue >> (i * 8)) & 0xFF));
	}
}
	
void RSA::generateP(std::promise<uint1024_t>&& promiseP)
{
	uint1024_t p = this->getRandomPrimeNumber<uint1024_t>();
	std::cout << "P generated!\n";
	promiseP.set_value(p);
	this->_generatedP = true;
}

void RSA::generateQ(std::promise<uint1024_t>&& promiseQ)
{
	uint1024_t q = this->getRandomPrimeNumber<uint1024_t>();
	std::cout << "Q generated!\n";
	promiseQ.set_value(q);
	this->_generatedQ = true;
}

uint2048_t RSA::calcProduct(const uint1024_t& q, const uint1024_t& p)
{
	return static_cast<uint2048_t>(uint2048_t(q) * uint2048_t(p));
}
	
uint2048_t RSA::calcTotient(const uint1024_t& q, const uint1024_t& p)
{
	return static_cast<uint2048_t>(uint2048_t(q - 1) * uint2048_t(p - 1));
}
	
void RSA::selectPublicKey()
{
	std::cout << "starting to generate pubkey\n";

	uint2048_t lb = uint2048_t("65000");
	uint2048_t ub = uint2048_t("500000");

	while (true)
	{
		E = this->getRandomPrimeNumber<uint2048_t>(lb, ub);

		if (E < T && E % T != 0)
		{
			std::cout << "finished to generate pubkey\n";
			return;
		}
	}
}
	
void RSA::selectPrivateKey()
{
	std::cout << "starting to generate privatekey\n";

	uint2048_t lb = 0;
	uint2048_t ub = uint2048_t(this->T);

	int bits = 1024;
	
	do
	{
		lb = uint2048_t(uint2048_t(2) << bits);
		bits--;
	} while (lb >= ub);

	cpp_int mul_saver;

	while (true)
	{
		D = this->getRandomPrimeNumber<uint2048_t>(lb, ub);

		mul_saver = D * E;

		std::cout << std::endl << std::endl;
		std::cout << mul_saver << " % " << (cpp_int)(T) << " = " << mul_saver % (cpp_int)(T) << " ==? 1\n\n";

		if (mul_saver % (cpp_int)(T) == (cpp_int)(1))
		{
			std::cout << "finished to generate privatekey\n";
			return;
		}
	}
}
