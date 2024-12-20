#include <iostream>
#include "../../utils/RSA.h"

int main()
{
	std::cout << "TEST BEGIN:" << std::endl;
	RSA rsa = RSA();

	std::vector<unsigned char> text;
	std::cout << "text before: Hello world!\n";
	for (unsigned char ch : "Hello world!") {
		text.emplace_back(ch);
	}
	vector<unsigned char> cipher = rsa.Encrypt(text);
	std::string text_to_print(cipher.begin(), cipher.end());
	std::cout << "cypher text: " << text_to_print << std::endl;
	vector<unsigned char> plain = rsa.Decrypt(cipher);
	std::string text_to_print2(plain.begin(), plain.end());
	std::cout << "text after: " << text_to_print2 << std::endl;
	std::cout << "length of text after: " << text_to_print2.length() << std::endl;
	if(text_to_print2.compare(std::string("Hello world!")))
		std::cout << "TEST END SUCCESSFULLY:" << std::endl;
	else std::cout << "TEST FAILD:" << std::endl;
	return 0;
}

