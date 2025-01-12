#pragma once 
#include "prime_numbers_manager.h"
#include "utils.hpp"
#include <thread>
#include <future>
class ECDHE
{
public:
	//create 
	ECDHE();
	~ECDHE();
	std::pair<uint256_t, std::pair<uint256_t, uint256_t>> createInfo(); //side a
	uint256_t createTmpKey(); //side b
	uint256_t createDefiKey(uint256_t base, uint256_t tmpKey, uint256_t moduler);
	uint256_t createDefiKey(uint256_t key);
	void setG(uint256_t g);
	void setP(uint256_t p);

private:
	uint256_t getRandomPrimeNumberByRange(uint256_t lower_bound = 1, uint256_t upper_bound = 1);
	void createElement(std::promise<uint256_t>&& promise);
	bool nullFlag;

	uint256_t g; 
	uint256_t p;
	//base- g, moduler - p : those are two public prims between 128 - 256 bits(size 256)
	//side1 key - a, side2 key - b : those are two private prims between 1 - 256 bits(size 256)
	//the shered seacret(the aes key of both side1 and side2) - value between 128 - 256 bits(more close to 256) (size 256)

};