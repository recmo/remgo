#pragma once
#include "Utilities.h"
#pragma GCC target ("sse4.1")
#define ssefunc __attribute__ ((__target__ ("sse4.1")))
typedef unsigned __int128 uint128;

#define const128(high,low)\
	(static_cast<uint128>(high) << 64 | static_cast<uint128>(low))

inline std::ostream& operator<<(std::ostream& out, const uint128& n)
{
	const uint64 high = n >> 64;
	const uint64 low = n;
	out << "const128(0x" << hex << high << "UL, 0x" << hex << low << "UL)";
	out << dec;
	return out;
}

inline uint popcount(uint128 n)
{
	const uint64 high = n >> 64;
	const uint64 low = n;
	return popcount(low) + popcount(high);
}

inline uint trailingZeros(uint128 n)
{
	const uint64 high = n >> 64;
	const uint64 low = n;
	if(low)
		return trailingZeros(low);
	else
		return 64 + trailingZeros(high);
}
