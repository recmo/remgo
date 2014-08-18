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

inline uint popcount(uint64 n)
{
	return __builtin_popcountll(n);
}

inline uint trailingZeros(uint64 n)
{
	return __builtin_ctzll(n);
}

#include "ForwardDeclare.h"
