#include "ECDHE.h"

//create 
ECDHE::ECDHE() {} // nothing to construct

ECDHE::~ECDHE() {} // nothing to diconstruct

void ECDHE::setG(const uint256_t g)
{
	_g = g;
}
void ECDHE::setP(const uint256_t p)
{
	_p = p;
}

const uint256_t ECDHE::getTmpKey()
{
	return _tmpKey;
}

std::pair<uint256_t, uint256_t> ECDHE::createInfo()
{
	std::promise<uint256_t> promiseG;
	std::promise<uint256_t> promiseP;

	std::future<uint256_t> futurePromiseG = promiseG.get_future();
	std::future<uint256_t> futurePromiseP = promiseP.get_future();

	std::thread generateGThread(&ECDHE::createElement, this, std::move(promiseG));
	std::thread generatePThread(&ECDHE::createElement, this, std::move(promiseP));

	createTmpKey();

	generateGThread.join();
	generatePThread.join();
		
	_g = futurePromiseG.get();
	_p = futurePromiseP.get();
	
	while (true)
	{
		try
		{
			if (!createDefiKey())
			{
				throw "calc result is 0!!! now fixing p (m)\n";
			}
			else break;
		}
		catch (std::runtime_error& e)
		{
			std::cout << e.what() << std::endl;
			createElement(std::move(promiseP));
			_p = futurePromiseP.get();
		}
	}
	
	return std::pair<uint256_t, uint256_t>(_g, _p);
}

void ECDHE::createTmpKey()
{
	uint256_t lowerBoned = uint256_t(2);
	uint256_t upperBoned = (uint256_t(1) << 256) - 1;
	_tmpKey = getRandomPrimeNumberByRange(lowerBoned, upperBoned);
}

uint256_t ECDHE::getRandomPrimeNumberByRange(uint256_t lower_bound, uint256_t upper_bound) 
{
	return prime_numbers_manager::getRandomPrimeNumber<uint256_t>(lower_bound, upper_bound);
}

void ECDHE::createElement(std::promise<uint256_t>&& promise)
{
	uint256_t lowerBoned = uint256_t(1) << 128;
	uint256_t upperBoned = (uint256_t(1) << 256) - 1;
	promise.set_value(getRandomPrimeNumberByRange(lowerBoned, upperBoned));
}

uint256_t ECDHE::createDefiKey(uint256_t base, uint256_t tmpKey, uint256_t moduler)
{
	return mod_exp<uint256_t>((cpp_int)(base), tmpKey, moduler);
}

uint256_t ECDHE::createDefiKey()
{
	return mod_exp<uint256_t>((cpp_int)(_g), _tmpKey, _p);
}