#include <iostream>
#include "../../utils/RSA.h"
#include <chrono>     

int main()
{
	std::chrono::time_point<std::chrono::system_clock> start;
	double timeElapsed;

	std::cout << "TEST BEGIN:" << std::endl;
	RSA rsa = RSA();

	std::vector<unsigned char> text;
	std::cout << "text before: Hello world!\n";

	for (unsigned char ch : "hello world!"){
		text.emplace_back(ch);
	}

	start = std::chrono::system_clock::now();
	vector<unsigned char> cipher = rsa.Encrypt(text);
	timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count();

	std::cout << "finished encription at " << timeElapsed << "total." << std::endl;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    


	//std::string text_to_print(cipher.begin(), cipher.end());
	//std::cout << "cypher text: " << text_to_print << std::endl;
	
	start = std::chrono::system_clock::now();
	vector<unsigned char> plain = rsa.Decrypt(cipher);
	timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count();

	std::cout << "finished decription at " << timeElapsed << "total." << std::endl;


	std::string text_to_print2(plain.begin(), plain.end());
	std::cout << "text after: " << text_to_print2 << std::endl;
	std::cout << "length of text after: " << text_to_print2.length() << std::endl;
	if(text_to_print2.compare(std::string("Hello world!")))
		std::cout << "TEST END SUCCESSFULLY:" << std::endl;
	else std::cout << "TEST FAILD:" << std::endl;
	return 0;
}

