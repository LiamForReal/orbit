#pragma once

#include "utils.hpp"
#include <random>
#include <vector>

class prime_numbers_manager
{
public:

    template<typename T>
    static T getRandomPrimeNumber(T lowerBond, T upperBond)
    {
        T lower_bound;
        T upper_bound;
        if (lowerBond != 1)
            lower_bound = lowerBond;
        if (upperBond != 1)
            upper_bound = upperBond;

        if (std::is_same<T, uint1024_t>::value)
        {
            std::cout << "is 1024\n";
            if (lowerBond == 1)
                lower_bound = T(1) << 512;
            if (upperBond == 1)
                upper_bound = (T(1) << 1024) - 1;
        }
        else if (std::is_same<T, uint2048_t>::value)
        {
            std::cout << "is 2048\n";
            if (lowerBond == 1)
                lower_bound = T(1) << 1024;
            if (upperBond == 1)
                upper_bound = (T(1) << 2048) - 1;
        }

        boost::random::mt19937 rng(std::random_device{}());
        boost::random::uniform_int_distribution<T> dist(lower_bound, upper_bound);

        T candidate;
        do
        {
            candidate = dist(rng);
            if (candidate % 2 == 0)
                candidate++;
        } while (!prime_numbers_manager::is_prime<T>(candidate) || prime_numbers_manager::is_divisible_by_small_primes<T>(candidate));

        return candidate;
    }

private:
    template<typename T>
    static bool is_prime(const T& n) {
        if (n <= 1) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;

        // Miller-Rabin primality test
        T d = n - 1;
        int r = 0;
        while (d % 2 == 0) {
            d /= 2;
            r++;
        }

        boost::random::mt19937 rng(std::random_device{}());
        boost::random::uniform_int_distribution<T> dist(2, n - 2);

        for (int i = 0; i < 5; ++i) {  // Perform 5 rounds of Miller-Rabin
            T a = dist(rng);
            T x = boost::multiprecision::powm(a, d, n);  // a^d % n
            if (x == 1 || x == n - 1) continue;

            bool is_composite = true;
            for (int j = 0; j < r - 1; ++j) {
                x = boost::multiprecision::powm(x, 2, n);  // x^2 % n
                if (x == n - 1) {
                    is_composite = false;
                    break;
                }
            }

            if (is_composite) {
                return false;
            }
        }
        return true;
    }

    template <typename T>
    static bool is_divisible_by_small_primes(const T& candidate) {
        // Precomputed list of small primes (first 10 primes)
        static const std::vector<T> small_primes = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };

        // Check divisibility against each small prime
        for (const auto& prime : small_primes) {
            if (candidate % prime == 0) {
                return true; // Candidate is divisible by a small prime
            }
        }

        return false; // Candidate is not divisible by any small prime
    }
};