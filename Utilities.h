#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <inttypes.h>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <cmath>
using namespace std;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef signed int sint;
typedef __uint128_t uint128;

inline uint popcount(uint64 n)
{
	return __builtin_popcountll(n);
}

inline uint trailingZeros(uint64 n)
{
	return __builtin_ctzll(n);
}

inline uint popcount(uint128 n)
{
	const uint64 low = n;
	const uint64 high = n >> 64;
	return popcount(low) + popcount(high);
}
inline uint trailingZeros(uint128 n)
{
	const uint64 low = n;
	const uint64 high = n >> 64;
	if(low)
		return trailingZeros(low);
	else
		return 64 + trailingZeros(high);
}

#include "ForwardDeclare.h"
