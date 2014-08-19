#pragma once
#include "Utilities.h"
#include <emmintrin.h>

class Uint128 {
public:
	explicit Uint128(uint64 low);
	explicit Uint128(uint64 high, uint64 low);
	Uint128(const Uint128& copy) : _v(copy._v) { }
	Uint128& operator=(const Uint128& copy) { _v = copy._v; return *this; }
	
	operator bool() const;
	operator uint64() const { return _low; }
	bool operator==(const Uint128& rhs) const;
	bool operator!=(const Uint128& rhs) const { return !operator==(rhs); }
	
	Uint128 operator&(const Uint128& rhs) const;
	Uint128 operator|(const Uint128& rhs) const;
	Uint128 operator^(const Uint128& rhs) const;
	Uint128& operator&=(const Uint128& rhs);
	Uint128& operator|=(const Uint128& rhs);
	Uint128& operator^=(const Uint128& rhs);
	Uint128 operator~() const;
	
	Uint128 operator<<(int rhs) const;
	Uint128 operator>>(int rhs) const;
	Uint128& operator<<=(int rhs) { return operator=(*this << rhs); }
	Uint128& operator>>=(int rhs) { return operator=(*this >> rhs); }
	
protected:
	explicit Uint128(__m128i v): _v(v) { }
	friend uint popcount(Uint128 n);
	friend uint trailingZeros(Uint128 n);
	__m128i _v;
};

inline bool Uint128::operator bool() const
{
	static const __m128i zero = {0};
	return !_mm_testc_si128(zero, _v);
}

inline bool Uint128::operator==(const Uint128& rhs) const
{
	return static_cast<bool>(*this ^ rhs);
}

inline Uint128 Uint128::operator&(const Uint128& rhs) const
{
	return Uint128(_high & rhs._high, _low & rhs._low);
}

inline Uint128 Uint128::operator|(const Uint128& rhs) const
{
	return Uint128(_high | rhs._high, _low | rhs._low);
}

inline Uint128 Uint128::operator^(const Uint128& rhs) const
{
	return Uint128(_high ^ rhs._high, _low ^ rhs._low);
}

inline Uint128& Uint128::operator&=(const Uint128& rhs)
{
	_high &= rhs._high;
	_low &= rhs._low;
	return *this;
}

inline Uint128& Uint128::operator|=(const Uint128& rhs)
{
	_high |= rhs._high;
	_low |= rhs._low;
	return *this;
}

inline Uint128& Uint128::operator^=(const Uint128& rhs)
{
	_high ^= rhs._high;
	_low ^= rhs._low;
	return *this;
}

inline Uint128 Uint128::operator~() const
{
	return Uint128(~_high, ~_low);
}

// TODO: https://stackoverflow.com/questions/17610696/shift-a-m128i-of-n-bits

inline Uint128 Uint128::operator<<(int rhs) const
{
	__m128i v1, v2;
	if(rhs >= 64) {
		v1 = _mm_slli_si128(_v, 8);
		v1 = _mm_slli_epi64(v1, rhs - 64);
	} else {
		v1 = _mm_slli_epi64(_v, rhs);
		v2 = _mm_slli_si128(_v, 8);
		v2 = _mm_srli_epi64(v2, 64 - rhs);
		v1 = _mm_or_si128(v1, v2);
	}
	return Uint128(v1);
}

inline Uint128 Uint128::operator>>(int rhs) const
{
	__m128i v1, v2;
	if(rhs >= 64) {
		v1 = _mm_srli_si128(_v, 8);
		v1 = _mm_srli_epi64(v1, rhs - 64);
	} else {
		v1 = _mm_srli_epi64(_v, rhs);
		v2 = _mm_srli_si128(_v, 8);
		v2 = _mm_slli_epi64(v2, 64 - rhs);
		v1 = _mm_or_si128(v1, v2);
	}
	return Uint128(v1);
}

typedef Uint128 uint128;

#define const128(high,low)\
	Uint128(high, low)

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
	return popcount(n._low) + popcount(n._high);
}

inline uint trailingZeros(uint128 n)
{
	if(n._low)
		return trailingZeros(n._low);
	else
		return 64 + trailingZeros(n._high);
}
