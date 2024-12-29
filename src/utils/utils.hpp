#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp> // For the mt19937 engine
#include <boost/random/uniform_int_distribution.hpp> // For uniform integer distribution
#include "json.hpp"

using uint2048_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<2048, 2048, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>, boost::multiprecision::et_off>;
using uint1024_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<1024, 1024, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>, boost::multiprecision::et_off>;
using uint256_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<256, 256, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>, boost::multiprecision::et_off>;
using boost::multiprecision::cpp_int;

template <typename T>
static T mod_exp(T inputBase, T inputPower, T inputMod)
{
    cpp_int result = 1;
    cpp_int base = inputBase;
    cpp_int power = inputPower;
    cpp_int mod = inputMod;

    base = base % mod;

    if (base == 0) return 0;

    while (power > 0)
    {
        if (power & 1) // check if power is odd
        {
            result = (result * base) % mod;
        }

        power >>= 1; // divide power by 2 (efficient bitwise way)
        base = (base * base) % mod;
    }

    return (T)result;
}

template <typename T>
static T mod_exp_de(T inputBase, T inputPower, T inputMod)
{
    // Use the result, base, power, and mod variables for clarity and optimization
    T result = 1;
    T base = inputBase % inputMod; // Reduce base modulo inputMod immediately
    T power = inputPower;

    if (base == 0) return 0; // Shortcut for zero base

    while (power > 0)
    {
        // If the current power is odd, multiply result by the current base
        if (power & 1)
        {
            result = (result * base) % inputMod;
        }

        // Square the base and reduce it modulo inputMod
        base = (base * base) % inputMod;

        // Divide the power by 2
        power >>= 1;
    }

    return result;
}
