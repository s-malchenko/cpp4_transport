#pragma once

#include <cmath>

inline bool doublesEqual(long double lhs, long double rhs, long double precision = 0.01)
{
    return std::abs(lhs - rhs) <= precision;
}
