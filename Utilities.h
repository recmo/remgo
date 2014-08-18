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

#define const128(high,low)\
	((static_cast<uint128>(high) << 64) | static_cast<uint128>(low))

inline std::ostream& operator<<(std::ostream& out, const uint128& n)
{
	const uint64 high = n >> 64;
	const uint64 low = n;
	out << "const128(0x" << hex << high << "UL, 0x" << hex << low << "UL)";
	out << dec;
	return out;
}

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

template<class T>
std::ostream& operator<<(std::ostream& out, const vector<T>& vec)
{
	out << "[";
	bool first = true;
	for(auto v: vec) {
		if(!first)
			out << ", ";
		out << v;
		first = false;
	}
	out << "]";
	return out;
}

#include "ForwardDeclare.h"
