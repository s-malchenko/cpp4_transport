#pragma once

#include <cmath>

using namespace std;

inline bool doublesEqual(long double lhs, long double rhs, long double precision = 0.0001)
{
    return abs(lhs - rhs) <= precision;
}
