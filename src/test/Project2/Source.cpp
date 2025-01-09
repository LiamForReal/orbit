#include <iostream>
#include "../../utils/ECDHE.h"
int main()
{
	ECDHE ecdhe;
	std::cout << "start creating side1 g (base), p (moduler), tmpKey (a):\n";
	std::pair<uint256_t, std::pair<uint256_t, uint256_t>> ecdheInfo = ecdhe.createInfo();
	uint256_t base = ecdheInfo.second.first, moduler = ecdheInfo.second.second, tmpSide1Key = ecdheInfo.first;
	std::cout << "base = " << base << "\n";
	std::cout << "modulae = " << moduler << "\n";
	std::cout << "tmpKey = " << tmpSide1Key << "\n";

	std::cout << "start creating side2 tmpKey (b):\n";
	uint256_t tmpSide2Key = ecdhe.createTmpKey();
	std::cout << "tmpKey = " << tmpSide2Key << "\n";

	std::cout << "creating side1 and 2 ecdheKey (A, B):\n";
	uint256_t defiKeySide1 = ecdhe.createDefiKey(base, tmpSide1Key, moduler);
	uint256_t defiKeySide2 = ecdhe.createDefiKey(base, tmpSide2Key, moduler);
	std::cout << "defiKeySide1: " << defiKeySide1 << "\ndefiKeySide2: " << defiKeySide2 << "\n";

	std::cout << "creating side1 and 2 shered seacret (s):\n";
	uint256_t SheredSeacretSide1 = ecdhe.createDefiKey(defiKeySide2, tmpSide1Key, moduler);
	uint256_t  SheredSeacretSide2 = ecdhe.createDefiKey(defiKeySide1, tmpSide2Key, moduler);
	std::cout << "SheredSeacret: " << SheredSeacretSide1 << "\n";
	if (SheredSeacretSide1 == SheredSeacretSide2)
		std::cout << "TEST DONE SUCCESSFULLY!!! :D";
	else std::cout << "TEST FAILD :(";
	return 0;
}