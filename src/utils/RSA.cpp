#include "RSA.h"
#include <boost/integer/mod_inverse.hpp>

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
		// wait until the P & Q generation threads end.
	}

	uint1024_t q = futurePromiseQ.get();
	uint1024_t p = futurePromiseP.get();

	std::cout << "q = " << q << std::endl << "p = " << p << std::endl;
	std::cout << "p and q generated successfully\n";

	this->N = calcProduct(q, p);
	std::cout << "N (product) = " << this->N << std::endl;
	this->T = calcTotient(q, p);
	std::cout << "T (totient) = " << this->T << std::endl;
	selectPublicKey();
	std::cout << "E (public key) = " << this->E << std::endl;
	selectPrivateKey();
	std::cout << "D (private key) = " << this->D << std::endl;
	std::cout << "done making public and private keys!!!\n";

	this->P = uint2048_t(p);
	this->Q = uint2048_t(q);
	this->DP = this->D % (this->P - 1);
	this->DQ = this->D % (this->Q - 1);
	cpp_int pcopy = cpp_int(this->P);
	cpp_int qcopy = cpp_int(this->Q);

	cpp_int x0 = 1;
	cpp_int y0 = 0;

	cpp_int x1 = 0;
	cpp_int y1 = 1;

	cpp_int dividend = 0;
	cpp_int remainder = 0;

	do
	{
		if (qcopy > pcopy)
		{
			dividend = qcopy / pcopy;
			remainder = qcopy % pcopy;

			x1 = x1 - dividend * x0;
			y1 = y1 - dividend * y0;

			qcopy = remainder;
		}
		else
		{
			dividend = pcopy / qcopy;
			remainder = pcopy % qcopy;

			x0 = x0 - dividend * x1;
			y0 = y0 - dividend * y1;

			pcopy = remainder;
		}
	} while (pcopy > 0 && qcopy > 0);

	if (pcopy == 0)
	{
		// x0 is equals to totient, x1 is equals to private key
		this->QINV = (uint2048_t)(euclideanMod(x1, qcopy));
	}
	if (qcopy == 0)
	{
		// x0 is equals to private key, x1 is equals to -totient
		this->QINV = (uint2048_t)(euclideanMod(x0, qcopy));
	}

	std::cout << "Generated CRT params\n";
}

vector<unsigned char> RSA::Encrypt(vector<unsigned char>& plainTextVec)
{
	vector<unsigned char> cipherTextVec;
	cipherTextVec.reserve(plainTextVec.size() * 256);

	// Process each byte (or group of bytes if applicable)
	for (size_t i = 0; i < plainTextVec.size(); ++i)
	{
		unsigned char ch = plainTextVec[i];

		uint2048_t encryptedCh = (uint2048_t)mod_exp<uint2048_t>(ch, this->E, this->N);

		// Split the encrypted value into 256-byte chunks
		for (short j = 0; j < 256; j++)
		{
			unsigned char encryptedChPart = (unsigned char)(encryptedCh & 0xFF);
			cipherTextVec.emplace_back(encryptedChPart);
			encryptedCh >>= 8;
		}
	}

	return cipherTextVec;
}

vector<unsigned char> RSA::Decrypt(vector<unsigned char>& cipherTextVec) {
	vector<unsigned char> plainTextVec;
	plainTextVec.reserve(cipherTextVec.size() / 256);
	uint2048_t encryptedCh, decryptedCh;

	for (size_t i = 0; i < cipherTextVec.size(); i += 256) {
		encryptedCh = 0;

		// Rebuild the encrypted message (uint2048_t) from 256 bytes
		for (short j = 255; j != 0; --j) {
			encryptedCh = (encryptedCh << 8) | cipherTextVec[i + j];
		}

		// Compute m1 and m2
		uint2048_t m1 = mod_exp<uint2048_t>(encryptedCh, this->DP, this->P);
		uint2048_t m2 = mod_exp<uint2048_t>(encryptedCh, this->DQ, this->Q);

		// Combine m1 and m2 using CRT
		uint2048_t h = (this->QINV * (m1 - m2 + this->P)) % this->P;
		decryptedCh = m2 + h * this->Q;

		// Decrypted value is expected to be a single byte
		unsigned char decryptedByte = (unsigned char)(decryptedCh & 0xFF);
		plainTextVec.emplace_back(decryptedByte);
	}

	return plainTextVec;
}


	
cpp_int RSA::euclideanMod(const cpp_int& num, const cpp_int& mod)
{
	return ((num % mod + mod) % mod);
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

	cpp_int E = this->E;
	cpp_int T = this->T;

	cpp_int x0 = 1;
	cpp_int y0 = 0;

	cpp_int x1 = 0;
	cpp_int y1 = 1;

	cpp_int dividend = 0;
	cpp_int remainder = 0;

	do
	{
		if (T > E)
		{
			dividend = T / E;
			remainder = T % E;

			x1 = x1 - dividend * x0;
			y1 = y1 - dividend * y0;

			T = remainder;
		}
		else
		{
			dividend = E / T;
			remainder = E % T;

			x0 = x0 - dividend * x1;
			y0 = y0 - dividend * y1;

			E = remainder;
		}
	} while (E > 0 && T > 0);

	std::cout << "E is 0\n";

	if (E == 0)
	{
		// x0 is equals to totient, x1 is equals to private key
		this->D = (uint2048_t)(euclideanMod(x1, this->T));
	}
	if (T == 0) 
	{
		// x0 is equals to private key, x1 is equals to -totient
		this->D = (uint2048_t)(euclideanMod(x0, this->T));
	}

	// std::cout << this->D << std::endl << std::endl;
	// cpp_int mulmod = cpp_int(this->D) * cpp_int(this->E) % cpp_int(this->T);
	// std::cout << this->D << " * " << this->E << " % " << this->T << " = " << (mulmod) << std::endl;

	std::cout << "Finished to generate private key\n";
}
