#pragma once 
#include "prime_numbers_manager.h"
class ECDHE
{
public:
	//create 
	ECDHE();
	~ECDHE();
	std::pair<uint256_t, std::pair<uint256_t, uint256_t>> createInfo();
private:
	uint256_t getRandomPrimeNumberByRange(uint256_t lower_bound = 1, uint256_t upper_bound = 1);
	uint256_t createP();
	uint256_t createG();
	uint256_t createKey();
	//baser - g, moduler - p : those are two public prims between 128 - 256 bits(size 256)
	//side1 key - a, side2 key - b : those are two private prims between 1 - 256 bits(size 256)
	//the shered seacret(the aes key of both side1 and side2) - value between 128 - 256 bits(more close to 256) (size 256)

};