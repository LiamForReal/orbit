#include "AES.h"

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
