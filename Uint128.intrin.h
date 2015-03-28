#pragma once
#include <ostream>
#pragma GCC target ("sse4.1")
#pragma GCC optimize ("O3")
#define funk __attribute__ ((__target__ ("sse4.1"), optimize("O3")))
typedef u_int64_t uint64;
typedef unsigned __int128 uint128;

#define const128(high,low)\
	(static_cast<uint128>(high) << 64 | static_cast<uint128>(low))

uint popcount(uint128 n) funk;
uint trailingZeros(uint128 n) funk;

inline std::ostream& operator<<(std::ostream& out, const uint128& n)
{
	const uint64 high = n >> 64;
	const uint64 low = n;
	out << "const128(0x" << std::hex << high << "UL, 0x" << std::hex << low << "UL)";
	out << std::dec;
	return out;
}

inline uint popcount(uint128 n)
{
	const uint64 high = n >> 64;
	const uint64 low = n;
	return __builtin_popcountll(low) + __builtin_popcountll(high);
}

inline uint trailingZeros(uint128 n)
{
	const uint64 high = n >> 64;
	const uint64 low = n;
	if(low)
		return __builtin_ctzll(low);
	else
		return 64 + __builtin_ctzll(high);
}
