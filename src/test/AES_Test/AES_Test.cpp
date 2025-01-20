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

    cvec.clear();

    system("pause");
    return 0;
}