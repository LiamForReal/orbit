#include "ECDHE.h"

//create 
ECDHE::ECDHE() {} // nothing to construct

ECDHE::~ECDHE() {} // nothing to diconstruct

std::pair<uint256_t, std::pair<uint256_t, uint256_t>> ECDHE::createInfo()
{
	std::cout << "1";
	std::promise<uint256_t> promiseG;
	std::promise<uint256_t> promiseP;

	std::cout << "2";
	std::future<uint256_t> futurePromiseG = promiseG.get_future();
	std::future<uint256_t> futurePromiseP = promiseP.get_future();

	std::cout << "3";
	std::thread generateQThread(&ECDHE::createElement, this, std::move(promiseG));
	std::thread generatePThread(&ECDHE::createElement, this, std::move(promiseP));
	
	uint256_t tmpKey = createTmpKey();
	std::cout << "4";
	generateQThread.join();
	generatePThread.join();
	std::cout << "5";
	std::pair<uint256_t, uint256_t> gAndP = std::pair<uint256_t, uint256_t>(futurePromiseG.get(), futurePromiseP.get());
	std::cout << "6";
	return std::pair<uint256_t, std::pair<uint256_t, uint256_t>>(tmpKey, gAndP);
}

uint256_t ECDHE::createTmpKey()
{
	uint256_t lowerBoned = uint256_t(1);
	uint256_t upperBoned = (uint256_t(1) << 256) - 1;
	return getRandomPrimeNumberByRange(lowerBoned, upperBoned);
}

uint256_t ECDHE::getRandomPrimeNumberByRange(uint256_t lower_bound, uint256_t upper_bound) 
{
	return prime_numbers_manager::getRandomPrimeNumber<uint256_t>(lower_bound, upper_bound);
}

uint256_t ECDHE::createElement(std::promise<uint256_t>&& promise)
{
	uint256_t lowerBoned = uint256_t(1) << 128;
	uint256_t upperBoned = (uint256_t(1) << 256) - 1;
	return getRandomPrimeNumberByRange(lowerBoned, upperBoned);
}

uint256_t ECDHE::createDefiKey(uint256_t base, uint256_t tmpKey, uint256_t moduler)
{
	return mod_exp<uint256_t>((cpp_int)(base), tmpKey, moduler);
}