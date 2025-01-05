#pragma once

#include "../../utils/utils.hpp"
#include <vector>
#include <map>

/* AES chunk / block size in bytes */
#define AES_CHUNK_SIZE_BYTES 16

/* 14 + 1 AES rounds, because it is AES-256*/
#define AES_ROUNDS 15

#define AES_ROUND_KEYS_ROWS 4
#define AES_ROUND_KEYS_COLS 8

/* AES uses column-major ordered 4 x 4 grid */
#define AES_GRID_ROWS 4
#define AES_GRID_COLS 4

class AES
{
	public:
		~AES();
		void generateRoundKeys();

		std::vector<uint8_t> encrypt(std::vector<uint8_t> plainTextVec);

	private:
		void rotWord(const uint8_t& key, const uint8_t& col);

		void addRoundKey();
		void subBytes();
		void mixColumns();
		void shiftRows();

		uint256_t _key;
		std::map<uint8_t, uint8_t[AES_ROUND_KEYS_ROWS][AES_ROUND_KEYS_COLS]> _roundKeys;
};

