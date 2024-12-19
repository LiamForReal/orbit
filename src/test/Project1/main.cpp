#include <iostream>
#include "../../utils/RSA.h"

int main()
{
	std::cout << "TEST BEGIN:" << std::endl;
	RSA rsa = RSA();

	return 0;

	std::vector<unsigned char> text;
	std::cout << "text before: Hello world!\n";
	for (unsigned char ch : "Hello world!") {
		text.emplace_back(ch);
	}
	rsa.Encrypt(text);
	std::string text_to_print(text.begin(), text.end());
	std::cout << "cypher text: " << text_to_print << std::endl;
	rsa.Decrypt(text);
	std::string text_to_print2(text.begin(), text.end());
	std::cout << "text after: " << text_to_print2 << std::endl;
	if("Hello world!" == text_to_print2)
		std::cout << "TEST END SUCCESSFULLY:" << std::endl;
	else std::cout << "TEST FAILD:" << std::endl;
	return 0;
}

