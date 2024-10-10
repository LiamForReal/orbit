#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>

using uint4096_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<4096, 4096, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>, boost::multiprecision::et_off>;

int main()
{
    uint4096_t n = 0xFFFFFFFFFFFFFFF;
    for (int i = 0; i < 400; i++)
    {
        n *= 0xFFFFFFFFFFFFFFF;
    }
	
    std::cout << n << std::endl;
	
	system("pause");
    return 0;
}