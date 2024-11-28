#pragma once

#include "utils.hpp"

class prime_numbers_manager
{
public:
	static uint2048_t getRandomPrimeNumber();
	
private:
	void buildPrimeFile();
	/*add variables if needed*/
};