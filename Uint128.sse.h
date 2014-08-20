#pragma once
#include <ostream>
#pragma GCC target ("sse4.1")
#pragma GCC optimize ("O3")
#define funk __attribute__ ((__target__ ("sse4.1"), optimize("O3")))
#define __MMX__
#define __SSE__
#define __SSE2__
#define __SSE3__
#define __SSSE3__
#define __SSE4_1__
#include <smmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>
typedef __m128i m128;

class Uint128 {
public:
	Uint128() funk;
	explicit Uint128(u_int64_t low) funk;
	explicit Uint128(u_int64_t high, u_int64_t low) funk;
	Uint128(const Uint128& copy) funk: _v(copy._v) { }
	Uint128& operator=(const Uint128& copy)funk { _v = copy._v; return *this; }
	
	operator bool() const funk { return !isZero(); }
	void clear() funk;
	bool isZero() const funk;
	bool operator==(const Uint128& rhs) const funk;
	bool operator!=(const Uint128& rhs) const funk { return !operator==(rhs); }
	
	Uint128 operator&(const Uint128& rhs) const funk;
	Uint128 operator|(const Uint128& rhs) const funk;
	Uint128 operator^(const Uint128& rhs) const funk;
	Uint128 operator-(const Uint128& rhs) const funk;
	Uint128& operator&=(const Uint128& rhs) funk;
	Uint128& operator|=(const Uint128& rhs) funk;
	Uint128& operator^=(const Uint128& rhs) funk;
	Uint128& operator-=(const Uint128& rhs) funk;
	Uint128 operator~() const funk;
	
	Uint128 operator<<(int rhs) const funk;
	Uint128 operator>>(int rhs) const funk;
	Uint128& operator<<=(int rhs) funk { return operator=(*this << rhs); }
	Uint128& operator>>=(int rhs) funk { return operator=(*this >> rhs); }
	
	u_int64_t high() const funk;
	u_int64_t low() const funk;
	
protected:
	explicit Uint128(m128 v) funk: _v(v) { }
	friend uint popcount(Uint128 n) funk;
	friend uint trailingZeros(Uint128 n) funk;
	m128 _v;
};

#define const128(high,low)\
	Uint128(high, low)

typedef Uint128 uint128;

std::ostream& operator<<(std::ostream& out, const uint128& n) funk;
uint popcount(uint128 n) funk;
uint trailingZeros(uint128 n) funk;
m128 mm_set_ones() funk;

inline m128 mm_set_ones()
{
	m128 junk;
	return _mm_cmpeq_epi8(junk, junk);
}

inline Uint128::Uint128()
: _v(_mm_setzero_si128())
{
}

inline Uint128::Uint128(u_int64_t low)
: _v(_mm_set_epi64x(0, low))
{
}

inline Uint128::Uint128(u_int64_t high, u_int64_t low)
: _v(_mm_set_epi64x(high, low))
{
}

inline void Uint128::clear()
{
	_v = _mm_setzero_si128();
}

inline bool Uint128::isZero() const
{
	return _mm_testz_si128(_v, _v);
}

inline bool Uint128::operator==(const Uint128& rhs) const
{
	return (*this ^ rhs).isZero();
}

inline Uint128 Uint128::operator&(const Uint128& rhs) const
{
	return Uint128(_mm_and_si128(_v, rhs._v));
}

inline Uint128 Uint128::operator|(const Uint128& rhs) const
{
	return Uint128(_mm_or_si128(_v, rhs._v));
}

inline Uint128 Uint128::operator^(const Uint128& rhs) const
{
	return Uint128(_mm_xor_si128(_v, rhs._v));
}

inline Uint128 Uint128::operator-(const Uint128& rhs) const
{
	return Uint128(_mm_andnot_si128(rhs._v, _v));
}

inline Uint128& Uint128::operator&=(const Uint128& rhs)
{
	_v = _mm_and_si128(_v, rhs._v);
	return *this;
}

inline Uint128& Uint128::operator|=(const Uint128& rhs)
{
	_v = _mm_or_si128(_v, rhs._v);
	return *this;
}

inline Uint128& Uint128::operator^=(const Uint128& rhs)
{
	_v = _mm_xor_si128(_v, rhs._v);
	return *this;
}

inline Uint128& Uint128::operator-=(const Uint128& rhs)
{
	_v = _mm_andnot_si128(rhs._v, _v);
	return *this;
}

inline Uint128 Uint128::operator~() const
{
	return Uint128(_mm_xor_si128(_v, _mm_cmpeq_epi8(_v,_v)));
}

// TODO: https://stackoverflow.com/questions/17610696/shift-a-m128i-of-n-bits

#define Do_1_7  Do(1) Do(2) Do(3) Do(4) Do(5) Do(6) Do(7)

#define Do_1_15 Do(1) Do(2) Do(3) Do(4) Do(5) Do(6) Do(7) Do(8) \
                Do(9) Do(10) Do(11) Do(12) Do(13) Do(14) Do(15)


m128 shiftLeft(m128 x, int nbits) funk;
m128 shl8(m128 x, int nbytes) funk;
m128 shr64x(m128 x, int nbits) funk;

inline m128 shiftLeft(m128 x, int nbits)
{
    nbits &= 127;
    if (nbits > 7)
        x = shl8(x, nbits >> 3), nbits &= 7;
    if (nbits)
        x = _mm_or_si128(x, _mm_slli_si128(shr64x(x, nbits), 8));
    return  x;
}

inline m128 shl8(m128 x, int nbytes)
{
    switch (nbytes) {
#   undef   Do
#   define  Do(z) case z: return _mm_slli_si128(x, z);
    Do_1_15;
    }
    return x;
}

inline m128 shr64x(m128 x, int nbits)
{
    switch (nbits) {
#   undef   Do
#   define  Do(z)   case z: return _mm_srli_epi64(x, 64 - z);
    Do_1_7;
    }
    return x;
}

inline Uint128 Uint128::operator<<(int rhs) const
{
	m128 v1, v2;
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
	return uint128(shiftLeft(_v, rhs));
}

inline Uint128 Uint128::operator>>(int rhs) const
{
	m128 v1, v2;
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

inline u_int64_t Uint128::high() const
{
	const u_int64_t* bits = reinterpret_cast<const u_int64_t*>(&_v);
	return bits[1];
	// return _mm_cvtsi128_si64x(_mm_srli_si128(_v, 8));
}

inline u_int64_t Uint128::low() const
{
	const u_int64_t* bits = reinterpret_cast<const u_int64_t*>(&_v);
	return bits[0];
	// return _mm_cvtsi128_si64x(_v);
}

inline std::ostream& operator<<(std::ostream& out, const uint128& n)
{
	out << "const128(0x" << std::hex << n.high() << "UL, 0x" << std::hex << n.low() << "UL)";
	out << std::dec;
	return out;
}

inline uint popcount(uint128 n)
{
	const m128 lut = _mm_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4);
	const m128 msk = _mm_set1_epi8(0xf);
	m128 highNibles = _mm_and_si128(msk, _mm_srli_epi16(n._v, 4));
	m128 lowNibles = _mm_and_si128(msk, n._v);
	m128 highCount = _mm_shuffle_epi8(lut, highNibles);
	m128 lowCount = _mm_shuffle_epi8(lut, lowNibles);
	m128 counts = _mm_add_epi8(highCount, lowCount);
	m128 sums = _mm_sad_epu8(counts, _mm_setzero_si128());
	return _mm_cvtsi128_si64x(_mm_srli_si128(sums, 8)) + _mm_cvtsi128_si64x(sums);
}

inline uint trailingZeros(uint128 n)
{
	const u_int64_t l = n.low();
	return l ? __builtin_ctzll(l) : 64 + __builtin_ctzll(n.high());
}
