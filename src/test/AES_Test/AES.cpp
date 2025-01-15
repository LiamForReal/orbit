#include "AES.h"

AES::~AES()
{
    this->_roundKeys.clear();
}

void AES::generateRoundKeys()
{
    uint256_t keyCopy = this->_key;
    uint8_t roundKey = 1;
    uint8_t index = 1;

    //for (uint8_t i = 0; i < 8; i++)
    //{
    //    keyCopy += i;
    //    keyCopy <<= 8;
    //}

    keyCopy = 0;
    this->_roundKeys[0][0][0] = 0x48;
    this->_roundKeys[0][1][0] = 0x65;
    this->_roundKeys[0][2][0] = 0x6C;
    this->_roundKeys[0][3][0] = 0x6C;
    this->_roundKeys[0][0][1] = 0x6F;
    this->_roundKeys[0][1][1] = 0x20;
    this->_roundKeys[0][2][1] = 0x77;
    this->_roundKeys[0][3][1] = 0x6F;
    this->_roundKeys[0][0][2] = 0x72;
    this->_roundKeys[0][1][2] = 0x6C;
    this->_roundKeys[0][2][2] = 0x64;
    this->_roundKeys[0][3][2] = 0x21;

    //std::cout << "The key copy value is " << keyCopy << std::endl;

    //for (uint8_t i = 0; i < AES_ROUND_KEYS_COLS; i++)
    //{
    //    for (uint8_t j = 0; j < AES_ROUND_KEYS_ROWS; j++)
    //    {
    //        // std::cout << std::hex << ((uint8_t)(keyCopy) & 0xFF) << std::endl;
    //        this->_roundKeys[0][j][i] = (uint8_t)(keyCopy) & 0xFF;
    //        keyCopy >>= 8;
    //    }
    //}

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

    uint8_t* roundKeyColCopy = new uint8_t[AES_ROUND_KEYS_ROWS];

    for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; i++)
    {
        roundKeyColCopy[i] = this->_roundKeys[0][i][AES_ROUND_KEYS_COLS - DEC];
    }

    rotWord(roundKeyColCopy);
    subWord(roundKeyColCopy);
    addRoundConstant(roundKeyColCopy, index);

    for (uint8_t keyExpansionRound = 1; keyExpansionRound <= AES_KEY_EXPANSION_ROUNDS; keyExpansionRound++)
    {
        for (uint8_t col = 0; col < AES_ROUND_KEYS_COLS; col++)
        {
            for (uint8_t row = 0; row < AES_ROUND_KEYS_ROWS; row++)
            {
                this->_roundKeys[roundKey][row][col] = this->_roundKeys[roundKey - DEC][row][col] ^ roundKeyColCopy[row];
            }

            for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; i++)
            {
                roundKeyColCopy[i] = this->_roundKeys[roundKey][i][col];
            }

            if (3 == col)
            {
                subWord(roundKeyColCopy);
            }
            else if (7 == col)
            {
                index++;
                rotWord(roundKeyColCopy);
                subWord(roundKeyColCopy);
                addRoundConstant(roundKeyColCopy, index);
            }
        }

        roundKey++;
    }

    std::cout << "FINISHED KEY EXPANSION\n";

    for (uint8_t key = 0; key <= AES_KEY_EXPANSION_ROUNDS; key++)
    {
        std::cout << "<=== ROUND KEY " << int(key) << " START ===>\n";
        for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; i++)
        {
            for (uint8_t j = 0; j < AES_ROUND_KEYS_COLS; j++)
            {
                std::cout << std::hex << int(this->_roundKeys[key][i][j]) << "      ";
            }
            std::cout << std::endl;
        }
        std::cout << "<=== ROUND  " << int(key) << " END ===>\n";
    }

    delete[] roundKeyColCopy;
}

void AES::rotWord(uint8_t* roundKeyCol) 
{
    uint8_t temp = roundKeyCol[0];  // Save MSB
    for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; ++i) 
    {
        roundKeyCol[i] = roundKeyCol[i + INC];
    }
    roundKeyCol[AES_ROUND_KEYS_ROWS - DEC] = temp;  // Set MSB as LSB
}


void AES::subWord(uint8_t* roundKeyCol)
{
    for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; i++)
    {
        roundKeyCol[i] = this->SBOX[(roundKeyCol[i] >> 4) & 0x0F][roundKeyCol[i] & 0x0F];
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

void AES::addRoundConstant(uint8_t* roundKeyCol, const uint8_t& index)
{
    roundKeyCol[0] ^= roundConstant(index);
}

void AES::addRoundKey(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS], const uint8_t& round)
{
    uint8_t offset = (round % 2 == 0) ? 0 : 4;

    for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_COLS; j++)
        {
            grid[i][j] ^= this->_roundKeys[(((!round) ? 1 : round) - DEC) / 2][i][j + offset];
        }
    }
}

std::vector<uint8_t> AES::encrypt(std::vector<uint8_t> plainTextVec)
{
    std::vector<uint8_t> cipherTextVec;
    //std::vector<uint8_t> chunkVec;
    uint8_t chunkGrid[AES_GRID_ROWS][AES_GRID_COLS] = { { 0 } };
    
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

        addRoundKey(chunkGrid, 0);

        for (uint8_t round = 1; round <= AES_ROUNDS; round++)
        {

            addRoundKey(chunkGrid, round);
        }

        std::cout << "<=== CHUNK START ===>\n";
        for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
        {
            for (uint8_t j = 0; j < AES_GRID_COLS; j++)
            {
                std::cout << std::hex << int(chunkGrid[i][j]) << "      ";
            }
            std::cout << std::endl;
        }
        std::cout << "<=== CHUNK END ===>\n";

        //chunkVec.clear();
    }

    return cipherTextVec;
}
