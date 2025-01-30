// AES_Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AES.h"

int main()
{
    std::vector<uint8_t> vec;

    for (char c : "Hello world! My name is the best kid ever to exist (joking)...")
    {
        vec.emplace_back(c);
    }

    AES aes;
    aes.generateRoundKeys();
    std::vector<uint8_t> cvec = aes.encrypt(vec);

    for (uint8_t b : cvec)
    {
        std::cout << std::hex << int(b);
    }
    std::cout << std::endl;

    std::vector<uint8_t> pvec = aes.decrypt(cvec);
    std::string res = "";

    std::cout << "\n=== DECRYPTED DATA ===\n\n";

    for (uint8_t b : pvec)
    {
        std::cout << std::hex << int(b);
        res += char(b);

        if (res[res.length() - DEC] == NULL)
        {
            break;
        }
    }
    std::cout << "\nDECRYPTED STRING: " << std::endl << res.length() << std::endl;

    for (char c : res)
    {
        std::cout << c;
    }
    std::cout << "\n";

    for (int i = 0; i < res.length(); i++)
    {
        std::cout << res[i];
    }
    std::cout << "\n";

    cvec.clear();
    pvec.clear();
    vec.clear();

    system("pause");
    return 0;
}