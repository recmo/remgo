#pragma once
#include "utilities.h"

class Random {
public:
	Random();
	
	uint64 operator()(uint64 upper) { return next64() % upper; }
	
	float real();
	bool coinFlip(float probability);
	
private:
	friend wostream& operator<<(wostream& out, const Random& random);
	friend wistream& operator>>(wistream& in, Random& random);
	uint64 _a;
	uint64 _b;
	uint128 _seed;
	uint64 next64();
};

extern Random entropy;
wostream& operator<<(wostream& out, const Random& random);
wistream& operator>>(wostream& in, Random& random);

inline float Random::real()
{
	return static_cast<float>(next64()) / 1.84467440737096e19f;
}

inline bool Random::coinFlip(float probability)
{
	return real() < probability;
}

inline uint64 Random::next64()
{
	_a ^= _a << 23;
	_a ^= _a >> 17;
	_a ^= _b ^ (_b >> 26);
	swap(_a, _b);
	return _a + _b;
}
