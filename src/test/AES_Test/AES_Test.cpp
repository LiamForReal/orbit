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
    aes.encrypt(vec);

    system("pause");
    return 0;
}