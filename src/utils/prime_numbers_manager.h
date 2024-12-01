#pragma once

#include "utils.hpp"

class prime_numbers_manager
{
public:
	static uint2048_t getRandomPrimeNumber2048();
	static uint1024_t getRandomPrimeNumber1024();
	
private:
	void buildPrimeFile();
	/*add variables if needed*/
};