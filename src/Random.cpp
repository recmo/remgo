#include "Random.h"
#include <time.h>

void fmix128(uint64& h1, uint64& h2);

Random entropy;

Random::Random()
: _a(0xf4569e11f0711544ULL) // hardcoded random bits
, _b(0x40927e82880a682dULL) //
{
	// Create a unqiue seed
	_b ^= time(nullptr);
	fmix128(_a, _b);
	
	// Store and print the seed
	_seed = _a;
	_seed <<= 64;
	_seed |= _b;
	wcerr << "Random seed: " << *this << endl;
}

wostream& operator<<(wostream& out, const Random& random)
{
	out << random._seed;
	return out;
}

wistream& operator>>(wistream& in, Random& random)
{
	in >> random._seed;
	random._a = random._seed >> 64;
	random._b = random._seed;
	return in;
}

// MurMurHash3 mixing fuctions
inline void fmix64(uint64& k)
{
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdULL;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53ULL;
	k ^= k >> 33;
}

inline void fmix128(uint64& h1, uint64& h2)
{
	h1 += h2;
	h2 += h1;
	fmix64(h1);
	fmix64(h2);
	h1 += h2;
	h2 += h1;
}
