#pragma once
#include "Utilities.h"

class Uint128 {
public:
	explicit Uint128(uint64 low): _high(0), _low(low) { }
	explicit Uint128(uint64 high, uint64 low): _high(high), _low(low) { }
	Uint128(const Uint128& copy) : _high(copy._high), _low(copy._low) { }
	Uint128& operator=(const Uint128& copy) { _high = copy._high; _low = copy._low; return *this; }
	
	operator bool() const { return _high | _low; }
	operator uint64() const { return _low; }
	bool operator==(const Uint128& rhs) const { return _high == rhs._high && _low == rhs._low; }
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
	friend uint popcount(Uint128 n);
	friend uint trailingZeros(Uint128 n);
	uint64 _high;
	uint64 _low;
};

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
