#include "AES.h"

AES::~AES()
{
    this->_roundKeys.clear();
}

void AES::generateRoundKeys()
{
    uint256_t keyCopy = this->_key;

    for (uint8_t i = 0; i < 8; i++)
    {
        keyCopy += i;
        keyCopy <<= 8;
    }

    std::cout << "The key copy value is " << keyCopy << std::endl;

    for (uint8_t i = 0; i < AES_ROUND_KEYS_COLS; i++)
    {
        for (uint8_t j = 0; j < AES_ROUND_KEYS_ROWS; j++)
        {
            // std::cout << std::hex << ((uint8_t)(keyCopy) & 0xFF) << std::endl;
            this->_roundKeys[0][j][i] = (uint8_t)(keyCopy) & 0xFF;
            keyCopy >>= 8;
        }
    }

    std::cout << "<=== ROUND KEY START ===>\n";
    for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; i++)
    {
        for (uint8_t j = 0; j < AES_ROUND_KEYS_COLS; j++)
        {
            std::cout << std::hex << int(this->_roundKeys[0][i][j]) << "      ";
        }
        std::cout << std::endl;
    }
    std::cout << "<=== ROUND KEY END ===>\n";
}

void AES::rotWord(const uint8_t& key, const uint8_t& col)
{
    for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS - DEC; i++)
    {
        this->_roundKeys[key][i][col] ^= this->_roundKeys[key][i + INC][col];
        this->_roundKeys[key][i + INC][col] ^= this->_roundKeys[key][i][col];
        this->_roundKeys[key][i][col] ^= this->_roundKeys[key][i + INC][col];
    }
}

void AES::subWord(const uint8_t& key, const uint8_t& col)
{
    for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; i++)
    {
        this->_roundKeys[key][i][col] = this->SBOX[(this->_roundKeys[key][i][col] >> 4) & 0x0F][this->_roundKeys[key][i][col] & 0x0F];
    }
}

uint8_t AES::roundConstant(const uint8_t& i)
{
    if (i <= 1)
    {
        return 1;
    }
    else if (roundConstant(i - DEC) < 0x80)
    {
        return 2 * roundConstant(i - DEC);
    }
    return ((2 * roundConstant(i - DEC)) ^ 0x11B);
}

std::vector<uint8_t> AES::encrypt(std::vector<uint8_t> plainTextVec)
{
    std::vector<uint8_t> cipherTextVec;
    //std::vector<uint8_t> chunkVec;
    uint8_t chunkGrid[4][4] = { { 0 } };
    
    /* simple null padding */
    while (plainTextVec.size() % AES_CHUNK_SIZE_BYTES != 0)
    {
        plainTextVec.emplace_back(NULL);
    }
    
    //cipherTextVec.reserve(plainTextVec.size());
    
    for (auto it = plainTextVec.begin(); it != plainTextVec.end(); it += AES_CHUNK_SIZE_BYTES)
    {
        for (uint8_t i = 0; i < AES_GRID_COLS; i++)
        {
            for (uint8_t j = 0; j < AES_GRID_ROWS; j++)
            {
                chunkGrid[j][i] = *(it + j + AES_GRID_COLS * i);
            }
        }

        std::cout << "<=== CHUNK START ===>\n";
        for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
        {
            for (uint8_t j = 0; j < AES_GRID_COLS; j++)
            {
                std::cout << chunkGrid[i][j] << "      ";
            }
            std::cout << std::endl;
        }
        std::cout << "<=== CHUNK END ===>\n";

        //chunkVec.clear();
    }

    return cipherTextVec;
}
