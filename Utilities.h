#pragma once
#include "Uint128.sse.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <inttypes.h>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <sys/types.h>
#include <cstdint>
#include <algorithm>
using namespace std;
typedef signed char sing8;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef signed int sint;

inline uint popcount(uint64 n)
{
	return __builtin_popcountll(n);
}

inline uint trailingZeros(uint64 n)
{
	return __builtin_ctzll(n);
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

