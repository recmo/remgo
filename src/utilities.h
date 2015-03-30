#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <map>
#include <cassert>
#include <algorithm>
typedef signed char sint8;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef __uint128_t uint128;
typedef int32_t sint32;
typedef signed int sint;
#define aligned __attribute__((aligned(16)))

using std::wstring;
using std::wostream;
using std::wistream;
using std::wistringstream;
using std::wostringstream;
using std::getline;
using std::wcout;
using std::wcerr;
using std::wcin;
using std::endl;
using std::dec;
using std::hex;
using std::setw;
using std::setfill;
using std::vector;
using std::array;
using std::map;
using std::swap;

/*
// Aligned allocations
inline void* operator new(std::size_t sz)
{
	return memalign(16, sz);
}

inline void operator delete(void* ptr) noexcept
{
	free(ptr);
}
*/

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

inline uint popcount(uint128 n)
{
	const uint64 l = n;
	const uint64 h = n >> 64;
	return __builtin_popcountll(l) + __builtin_popcountll(h);
}

inline uint trailingZeros(uint64 n)
{
	return __builtin_ctzll(n);
}

inline uint trailingZeros(uint128 n)
{
	const uint64 l = n;
	if(l)
		return __builtin_ctzll(l);
	const uint64 h = n >> 64;
	if(h)
		return __builtin_ctzll(h) + 64;
	return 128;
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

wostream& operator<<(wostream& out, uint128 v);

template<class T>
wostream& operator<<(wostream& out, const vector<T>& vec)
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

inline uint64 nextRandom()
{
	static uint64 a = 0xf4569e11f0711544ULL;
	static uint64 b = 0x40927e82880a682dULL;
	a ^= a << 23;
	a ^= a >> 17;
	a ^= b ^ (b >> 26);
	swap(a, b);
	return a + b;
}

inline uint64 random(uint64 upper)
{
	return nextRandom() % upper;
}

float cpuTimeUsed();

float peakMemoryUsed();
