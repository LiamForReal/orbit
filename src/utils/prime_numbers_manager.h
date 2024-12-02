#pragma once

#include "utils.hpp"
#include <random>

class prime_numbers_manager
{
public:
	
    template<typename T>
    static T getRandomPrimeNumber()
    {
        T lower_bound = T(1) << 512;
        T upper_bound = (T(1) << 1024) - 1;

        boost::random::mt19937 rng(std::random_device{}());
        boost::random::uniform_int_distribution<T> dist(lower_bound, upper_bound);

        T candidate;

        do
        {
            candidate = dist(rng);
            if (candidate % 2 == 0)
                candidate += 1; // Ensure it's odd
        } while (prime_numbers_manager::is_prime<T>(candidate));

        return candidate;
    }
	
private:
    template<typename T>
    static bool is_prime(const T& n, int iterations = 20)
    {
        if (n < 2) return false;
        if (n == 2 || n == 3) return true;
        if (n % 2 == 0) return false;

        // Write n as d * 2^r + 1
        T d = n - 1;
        int r = 0;
        while (d % 2 == 0)
        {
            d /= 2;
            r++;
        }
        // Random number generator for witnesses
        boost::random::mt19937 rng(std::random_device{}());
        boost::random::uniform_int_distribution<T> dist(2, n - 2);
        uint2048_t a, x;
        for (int i = 0; i < iterations; ++i)
        {
            a = dist(rng); // Random witness
            x = powm(a, d, n); // Compute a^d % n

            if (x == 1 || x == n - 1) continue;

            bool is_composite = true;
            for (int j = 1; j < r; ++j) {
                x = powm(x, 2, n); // Square x
                if (x == n - 1) {
                    is_composite = false;
                    break;
                }
            }
            if (is_composite) return false;
        }
        return true;
    }
	/*add variables if needed*/
};