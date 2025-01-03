#pragma once

#include "../../utils/utils.hpp"
#include <vector>

/* AES chunk / block size in bytes */
#define AES_CHUNK_SIZE_BYTES 16
/* 14 AES rounds, because it is AES-256*/
#define AES_ROUNDS 14

/* AES uses column-major ordered 4 x 4 grid */
#define AES_GRID_ROWS 4
#define AES_GRID_COLS 4

class AES
{
	public:
		std::vector<uint8_t> encrypt(std::vector<uint8_t> plainTextVec);
		void addRoundKey();
		void subBytes();
		void mixColumns();
		void shiftRows();

	private:
		uint256_t _key;
};

