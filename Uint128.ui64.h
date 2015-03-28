#pragma once
#include <ostream>
#pragma GCC target ("sse4.1")
#pragma GCC optimize ("O3")
#define funk __attribute__ ((__target__ ("sse4.1"), optimize("O3")))
typedef u_int64_t uint64;

class Uint128 {
public:
	explicit Uint128(uint64 low) funk: _high(0), _low(low) { }
	explicit Uint128(uint64 high, uint64 low) funk: _high(high), _low(low) { }
	Uint128(const Uint128& copy) funk : _high(copy._high), _low(copy._low) { }
	Uint128& operator=(const Uint128& copy) funk { _high = copy._high; _low = copy._low; return *this; }
	
	operator bool() const funk { return _high | _low; }
	operator uint64() const funk { return _low; }
	bool operator==(const Uint128& rhs) const funk { return _high == rhs._high && _low == rhs._low; }
	bool operator!=(const Uint128& rhs) const funk { return !operator==(rhs); }
	
	Uint128 operator&(const Uint128& rhs) const funk;
	Uint128 operator|(const Uint128& rhs) const funk;
	Uint128 operator^(const Uint128& rhs) const funk;
	Uint128& operator&=(const Uint128& rhs) funk;
	Uint128& operator|=(const Uint128& rhs) funk;
	Uint128& operator^=(const Uint128& rhs) funk;
	Uint128 operator~() const funk;
	
	Uint128 operator<<(int rhs) const funk;
	Uint128 operator>>(int rhs) const funk;
	Uint128& operator<<=(int rhs) funk { return operator=(*this << rhs); }
	Uint128& operator>>=(int rhs) funk { return operator=(*this >> rhs); }
	
protected:
	friend uint popcount(Uint128 n) funk;
	friend uint trailingZeros(Uint128 n) funk;
	uint64 _high;
	uint64 _low;
};

uint popcount(Uint128 n) funk;
uint trailingZeros(Uint128 n) funk;

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
	if(rhs == 0)
		return *this;
	if(rhs < 64)
		return Uint128(
			(_high << rhs) | (_low >> (64 - rhs)),
			_low << rhs
		);
	else
		return Uint128(
			_low << (rhs - 64),
			0
		);
}

inline Uint128 Uint128::operator>>(int rhs) const
{
	if(rhs == 0)
		return *this;
	if(rhs < 64)
		return Uint128(
			_high >> rhs,
			(_low >> rhs) | (_high << (64 - rhs))
		);
	else
		return Uint128(
			0,
			_high >> (rhs - 64)
		);
}

typedef Uint128 uint128;

#define const128(high,low)\
	Uint128(high, low)

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
	return __builtin_popcountll(n._low) + __builtin_popcountll(n._high);
}


inline uint trailingZeros(uint128 n)
{
	if(n._low)
		return __builtin_ctzll(n._low);
	else
		return 64 + __builtin_ctzll(n._high);
}
