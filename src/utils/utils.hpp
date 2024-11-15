#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <cstring>
#include <vector>
#include "json.hpp"

using uint1024_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<1024, 1024, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>, boost::multiprecision::et_off>;