#include "AES.h"

AES::~AES()
{
    this->_roundKeys.clear();
}

AES::AES() {}

AES& AES::operator=(const AES& other)
{
    this->_roundKeys = other._roundKeys;
    this->_key = other._key;
    return *this;
}

bool AES::isInishialized() const
{
    return !_key.str().empty();
}

void AES::generateRoundKeys(uint256_t& key)
{
    this->_key = key;
    uint256_t keyCopy = this->_key;

    uint8_t roundKey = 1;
    uint8_t index = 1;

    for (uint8_t i = 0; i < AES_ROUND_KEYS_COLS; i++)
    {
        for (uint8_t j = 0; j < AES_ROUND_KEYS_ROWS; j++)
        {
            // std::cout << std::hex << ((uint8_t)(keyCopy) & 0xFF) << std::endl;
            this->_roundKeys[0][j][i] = (uint8_t)(keyCopy) & 0xFF;
            keyCopy >>= 8;
        }
    }

    //std::cout << "<=== ROUND KEY START ===>\n";
    //for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS; i++)
    //{
    //    for (uint8_t j = 0; j < AES_ROUND_KEYS_COLS; j++)
    //    {
    //        std::cout << std::hex << int(this->_roundKeys[0][i][j]) << "      ";
    //    }
    //    std::cout << std::endl;
    //}
    //std::cout << "<=== ROUND KEY END ===>\n";

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

    /* for (uint8_t key = 0; key <= AES_KEY_EXPANSION_ROUNDS; key++)
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
     }*/
    delete[] roundKeyColCopy;
}

void AES::rotWord(uint8_t* roundKeyCol)
{
    uint8_t temp = roundKeyCol[0];  // Save MSB
    for (uint8_t i = 0; i < AES_ROUND_KEYS_ROWS - DEC; ++i)
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

uint8_t AES::galoisMult(uint8_t a, uint8_t b) const
{
    uint8_t p = 0; /* accumulator for the product of the multiplication */
    while (a != 0 && b != 0)
    {
        if (b & 1) /* if the polynomial for b has a constant term, add the corresponding a to p */
        {
            p ^= a; /* addition in GF(2^m) is an XOR of the polynomial coefficients */
        }

        if (a & 0x80) /* GF modulo: if a has a nonzero term x^7, then must be reduced when it becomes x^8 */
        {
            a = (a << 1) ^ 0x11b; /* subtract (XOR) the primitive polynomial x^8 + x^4 + x^3 + x + 1 (0b1_0001_1011)  you can change it but it must be irreducible */
        }
        else
        {
            a <<= 1; /* equivalent to a*x */
        }
        b >>= 1;
    }
    return p;
}

void AES::addRoundKey(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS], const uint8_t& round)
{
    uint8_t offset = (round != 0 && ((round + INC) % 2 == 0 || round == 1)) ? 4 : 0;

    //std::cout << "ADDING ROUND KEY " << std::dec << int(round) << " WITH OFFEST " << std::dec << int(offset) << std::endl;

    for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_COLS; j++)
        {
            grid[i][j] ^= this->_roundKeys[round / 2][i][j + offset];
        }
    }
}

void AES::subBytes(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS])
{
    for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_COLS; j++)
        {
            grid[i][j] = this->SBOX[(grid[i][j] >> 4) & 0x0F][grid[i][j] & 0x0F];
        }
    }
}

void AES::shiftRows(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS])
{
    // Performs Left Shift
    for (uint8_t i = 1; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t k = 0; k < i; k++)
        {
            uint8_t temp = grid[i][0];  // Save MSB
            for (uint8_t j = 0; j < AES_GRID_COLS - DEC; j++)
            {
                grid[i][j] = grid[i][j + INC];
            }
            grid[i][AES_GRID_ROWS - DEC] = temp; // Set MSB as LSB
        }
    }
}

void AES::mixColumns(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS])
{
    uint8_t results[AES_GRID_ROWS][AES_GRID_COLS] = { {0} };

    for (uint8_t i = 0; i < AES_GRID_COLS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_ROWS; j++)
        {
            results[j][i] = 0;
            for (uint8_t k = 0; k < AES_GRID_COLS; k++)
            {
                results[j][i] ^= galoisMult(MIX_COLUMNS_MATRIX[j][k], grid[k][i]);
            }
        }
    }

    for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_COLS; j++)
        {
            grid[i][j] = results[i][j];
        }
    }
}

void AES::inverseSubBytes(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS])
{
    for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_COLS; j++)
        {
            grid[i][j] = this->INVERSE_SBOX[(grid[i][j] >> 4) & 0x0F][grid[i][j] & 0x0F];
        }
    }
}

void AES::inverseShiftRows(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS])
{
    // Performs Right Shift
    for (uint8_t i = 1; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t k = 0; k < i; k++)
        {
            uint8_t temp = grid[i][AES_GRID_ROWS - DEC];  // Save LSB
            for (uint8_t j = AES_GRID_COLS - DEC; j > 0; j--)
            {
                grid[i][j] = grid[i][j - DEC];
            }
            grid[i][0] = temp; // Set MSB as LSB
        }
    }
}

void AES::inverseMixColumns(uint8_t grid[AES_GRID_ROWS][AES_GRID_COLS])
{
    uint8_t results[AES_GRID_ROWS][AES_GRID_COLS] = { {0} };

    for (uint8_t i = 0; i < AES_GRID_COLS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_ROWS; j++)
        {
            results[j][i] = 0;
            for (uint8_t k = 0; k < AES_GRID_COLS; k++)
            {
                results[j][i] ^= galoisMult(INVERSE_MIX_COLUMNS_MATRIX[j][k], grid[k][i]);
            }
        }
    }

    for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
    {
        for (uint8_t j = 0; j < AES_GRID_COLS; j++)
        {
            grid[i][j] = results[i][j];
        }
    }
}

std::vector<uint8_t> AES::encrypt(std::vector<uint8_t> plainTextVec)
{
    std::vector<uint8_t> cipherTextVec;
    uint8_t chunkGrid[AES_GRID_ROWS][AES_GRID_COLS] = { { 0 } };

    /* simple null padding */
    while (plainTextVec.size() % AES_CHUNK_SIZE_BYTES != 0)
    {
        plainTextVec.emplace_back(NULL);
    }

    cipherTextVec.reserve(plainTextVec.size());

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
            subBytes(chunkGrid);
            shiftRows(chunkGrid);
            if (round != AES_ROUNDS)
            {
                mixColumns(chunkGrid);
            }
            addRoundKey(chunkGrid, round);
        }

        //std::cout << "<=== CHUNK START ===>\n";
        //for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
        //{
        //    for (uint8_t j = 0; j < AES_GRID_COLS; j++)
        //    {
        //        std::cout << std::hex << int(chunkGrid[i][j]) << "      ";
        //    }
        //    std::cout << std::endl;
        //}
        //std::cout << "<=== CHUNK END ===>\n";

        for (uint8_t i = 0; i < AES_GRID_COLS; i++)
        {
            for (uint8_t j = 0; j < AES_GRID_ROWS; j++)
            {
                cipherTextVec.emplace_back(chunkGrid[j][i]);
            }
        }
    }

    return cipherTextVec;
}

std::vector<uint8_t> AES::decrypt(std::vector<uint8_t> cipherTextVec)
{
    if (cipherTextVec.size() % 16 != 0)
        throw std::runtime_error("the decripted vector doesn't encripted");
    std::vector<uint8_t> plainTextVec;
    uint8_t chunkGrid[AES_GRID_ROWS][AES_GRID_COLS] = { { 0 } };

    plainTextVec.reserve(cipherTextVec.size());

    for (auto it = cipherTextVec.begin(); it != cipherTextVec.end(); it += AES_CHUNK_SIZE_BYTES)
    {
        for (uint8_t i = 0; i < AES_GRID_COLS; i++)
        {
            for (uint8_t j = 0; j < AES_GRID_ROWS; j++)
            {
                chunkGrid[j][i] = *(it + j + AES_GRID_COLS * i);
            }
        }

        addRoundKey(chunkGrid, AES_ROUNDS);

        for (uint8_t round = AES_ROUNDS; round > 0; round--)
        {
            inverseShiftRows(chunkGrid);
            inverseSubBytes(chunkGrid);
            addRoundKey(chunkGrid, round - DEC);
            if ((round - DEC))
            {
                inverseMixColumns(chunkGrid);
            }
        }

        /*std::cout << "<=== CHUNK START ===>\n";
        for (uint8_t i = 0; i < AES_GRID_ROWS; i++)
        {
            for (uint8_t j = 0; j < AES_GRID_COLS; j++)
            {
                std::cout << std::hex << int(chunkGrid[i][j]) << "      ";
            }
            std::cout << std::endl;
        }
        std::cout << "<=== CHUNK END ===>\n";*/

        for (uint8_t i = 0; i < AES_GRID_COLS; i++)
        {
            for (uint8_t j = 0; j < AES_GRID_ROWS; j++)
            {
                plainTextVec.emplace_back(chunkGrid[j][i]);
            }
        }
    }

    return plainTextVec;
}
