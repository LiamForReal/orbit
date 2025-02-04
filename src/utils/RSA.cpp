#include "RSA.h"

RSA::RSA()
{
	this->_generatedP = false;
	this->_generatedQ = false;
}

void RSA::pregenerateKeys(void)
{
	std::promise<uint1024_t> promiseQ;
	std::promise<uint1024_t> promiseP;

	std::future<uint1024_t> futurePromiseQ = promiseQ.get_future();
	std::future<uint1024_t> futurePromiseP = promiseP.get_future();

	std::thread generateQThread(&RSA::generateQ, this, std::move(promiseQ));
	std::thread generatePThread(&RSA::generateP, this, std::move(promiseP));

	generateQThread.join();
	generatePThread.join();

	uint1024_t q = futurePromiseQ.get();
	uint1024_t p = futurePromiseP.get();

	//std::cout << "Q = " << q << std::endl << "P = " << p << std::endl;
	this->N = calcProduct(q, p);
	//std::cout << "N (product) = " << this->N << std::endl;
	this->T = calcTotient(q, p);
	std::cout << "N and T created sucessfully!\n";
	//std::cout << "T (totient) = " << this->T << std::endl;
	selectPublicKey();
	std::cout << "E (public key) = " << this->E << std::endl;
	selectPrivateKey();
	//std::cout << "D (private key) = " << this->D << std::endl;
	std::cout << "done making public and private keys!!!\n";
	this->P = p;
	this->Q = q;
	this->DP = this->D % (this->P - 1);
	this->DQ = this->D % (this->Q - 1);
	cpp_int pcopy = this->P;
	cpp_int qcopy = this->Q;

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
		this->QINV = (uint2048_t)(euclideanMod(x1, this->Q));
	}
	if (qcopy == 0)
	{
		this->QINV = (uint2048_t)(euclideanMod(x0, this->Q));
	}
}

vector<unsigned char> RSA::Encrypt(vector<unsigned char> plainTextVec, const uint2048_t& pubkey, const uint2048_t& product)
{
	vector<unsigned char> cipherTextVec;

	// Process each byte (or group of bytes if applicable)
	for (size_t i = 0; i < plainTextVec.size(); ++i)
	{
		unsigned char ch = plainTextVec[i];

		uint2048_t encryptedCh = mod_exp<uint2048_t>(ch, pubkey, product);

		// Split the encrypted value into 256-byte chunks
		for (short j = 0; j < 256; j++)
		{
			unsigned char encryptedChPart = (unsigned char)(encryptedCh & 0xFF);
			cipherTextVec.push_back(encryptedChPart);
			encryptedCh >>= 8;
		}
	}

	return cipherTextVec;
}

vector<unsigned char> RSA::Encrypt(vector<unsigned char>& plainTextVec)
{
	vector<unsigned char> cipherTextVec;
	cipherTextVec.reserve(plainTextVec.size() * 256);

	// Process each byte (or group of bytes if applicable)
	for (size_t i = 0; i < plainTextVec.size(); ++i)
	{
		unsigned char ch = plainTextVec[i];

		uint2048_t encryptedCh = mod_exp<uint2048_t>(ch, this->E, this->N);

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

vector<unsigned char> RSA::Decrypt(vector<unsigned char>& cipherTextVec)
{
	if (cipherTextVec.size() % 256 != 0)
	{
		throw std::invalid_argument("Ciphertext size is not a multiple of block size (256 bytes).");
	}

	size_t numBlocks = cipherTextVec.size() / 256;
	vector<unsigned char> plainTextVec;
	plainTextVec.reserve(numBlocks);
	uint2048_t encryptedBlock = 0;
	uint2048_t decryptedBlock = 0;
	for (size_t i = 0; i < numBlocks; ++i)
	{
		encryptedBlock = 0;

		// Reassemble the 256-byte block into a uint2048_t value
		for (short j = 255; j >= 0; --j)
		{
			encryptedBlock = (encryptedBlock << 8) | cipherTextVec[(i * 256) + j];
		}

		// Decrypt the block using CRT
		decryptedBlock = CRTDecrypt(encryptedBlock);
		
		// Append decrypted byte to output
		if (decryptedBlock > 255)
		{
			
			throw std::runtime_error("Decrypted value exceeds valid byte range.");
		}
		std::cout << unsigned char(decryptedBlock);
		plainTextVec.emplace_back(static_cast<unsigned char>(decryptedBlock));
	}
	std::cout << "\n";
	return plainTextVec;
}

uint2048_t RSA::getPublicKey() const
{
	return this->E;
}

uint2048_t RSA::getProduct() const
{
	return this->N;
}

uint2048_t RSA::CRTDecrypt(uint2048_t& encryptedBlock)
{
	uint2048_t m1 = mod_exp<uint2048_t>(encryptedBlock, this->DP, this->P);
	uint2048_t m2 = mod_exp<uint2048_t>(encryptedBlock, this->DQ, this->Q);

	// Ensure (m1 - m2) is always positive by adding P before taking modulo P
	uint2048_t h = (this->QINV * ((m1 + P - m2) % P)) % P;
	uint2048_t decryptedBlock = m2 + h * this->Q;

	return decryptedBlock;
}

cpp_int RSA::euclideanMod(const cpp_int& num, const cpp_int& mod)
{
	cpp_int result = num % mod;
	if (result < 0)
	{
		result += mod;
	}
	return result;
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
	uint2048_t lb = uint2048_t(MIN_PUBLIC_KEY_VALUE);
	uint2048_t ub = uint2048_t(MAX_PUBLIC_KEY_VALUE);

	while (true)
	{
		E = this->getRandomPrimeNumber<uint2048_t>(lb, ub);
		if (E < T && boost::math::gcd(E, T) == 1) // Ensure E is coprime to T
		{
			std::cout << "Finished generating public key\n";
			return;
		}
	}
}

void RSA::selectPrivateKey()
{
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
	std::cout << "private key: " << this->D << "\n";
	// std::cout << this->D << std::endl << std::endl;
	// cpp_int mulmod = cpp_int(this->D) * cpp_int(this->E) % cpp_int(this->T);
	// std::cout << this->D << " * " << this->E << " % " << this->T << " = " << (mulmod) << std::endl;

}

uint2048_t RSA::mod_inverse(uint2048_t a, uint2048_t m)
{
	uint2048_t m0 = m, t, q;
	uint2048_t x0 = 0, x1 = 1;

	if (m == 1)
		return 0;

	while (a > 1) {
		q = a / m;
		t = m;

		m = a % m;
		a = t;
		t = x0;

		x0 = x1 - q * x0;
		x1 = t;
	}

	if (x1 < 0)
		x1 += m0;

	return x1;
}
