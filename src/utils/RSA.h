#pragma once 

#include "prime_numbers_manager.h"
#include <vector>
#include "utils.hpp"
#include <thread>
#include <future>
#define MIN_PUBLIC_KEY_VALUE "65000"
#define MAX_PUBLIC_KEY_VALUE "500000"
using std::vector;

class RSA
{
    public:
        RSA();

        void pregenerateKeys(void);

        static vector<unsigned char> Encrypt(vector<unsigned char> plainTextVec, const uint2048_t& pubkey, const uint2048_t& product);
        vector<unsigned char> Encrypt(vector<unsigned char>& plainTextVec);
        vector<unsigned char> Decrypt(vector<unsigned char>& cipherTextVec);

        uint2048_t getPublicKey() const;
        uint2048_t getProduct() const;

    protected:
        template <typename T>
        T getRandomPrimeNumber(T lowerBond = 1, T upperBond = 1)
        {
            return prime_numbers_manager::getRandomPrimeNumber<T>(lowerBond, upperBond);
        }


    private:
        cpp_int euclideanMod(const cpp_int& num, const cpp_int& mod);
        void generateP(std::promise<uint1024_t>&& promiseP);
        void generateQ(std::promise<uint1024_t>&& promiseQ);
        uint2048_t calcProduct(const uint1024_t& q, const uint1024_t& p);
        uint2048_t calcTotient(const uint1024_t& q, const uint1024_t& p);
        void selectPublicKey();
        void selectPrivateKey();
        uint2048_t CRTDecrypt(uint2048_t& encryptedBlock);
        uint2048_t mod_inverse(uint2048_t a, uint2048_t m);

        uint2048_t P;
        uint2048_t Q;
        uint2048_t E; //public_key;
        uint2048_t D; //private_key;
        uint2048_t T; //Totioent
        uint2048_t N; //Product
        uint2048_t DP;
        uint2048_t DQ;
        uint2048_t QINV;

        bool _generatedP;
        bool _generatedQ;
};