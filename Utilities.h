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
#include <stdlib.h>
#include <malloc.h>
using namespace std;
typedef signed char sint8;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int32_t sint32;
typedef signed int sint;
#define aligned __attribute__((aligned(16)))

// Aligned allocations
inline void* operator new(std::size_t sz)
{
	return memalign(16, sz);
}

inline void operator delete(void* ptr) noexcept
{
	free(ptr);
}

class CallTracer {
public:
	CallTracer(int line, const char* function);
	~CallTracer();
	
private:
	int _line;
	const char* _function;
};

inline CallTracer::CallTracer(int line, const char* function)
: _line(line)
, _function(function)
{
	std::cerr << ">> " << _function << ":" << _line << std::endl;
}

inline CallTracer::~CallTracer()
{
	std::cerr << "<< " << _function << ":" << _line << std::endl;
}

#define tracecall CallTracer callTracer(__LINE__, __PRETTY_FUNCTION__);

#define trace std::cerr << "-- " << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;


inline uint popcount(uint64 n)
{
	return __builtin_popcountll(n);
}

inline uint trailingZeros(uint64 n)
{
	return __builtin_ctzll(n);
}

inline uint64 rotate_left(uint64 a, int p)
{
	return (a << (p % 64)) | (a >> (64 - (p % 64)));
}

template<typename T>
T min(const T& a, const T& b, const T& c)
{
	return min(min(a, b), c);
}

template<typename T>
T min(const T& a, const T& b, const T& c, const T& d)
{
	return min(min(a, b), min(c, d));
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

