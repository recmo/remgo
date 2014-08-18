#include "Random.h"

float randomReal()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

uint32 entropy(uint upperBound)
{
	static std::mt19937_64 engine(time(0));
	const static uint64 full = (engine.max() - engine.min()) >> 8; // 8 bit safety margin
	static uint64 entropy = 0;
	static uint64 available = 0;
	
	// Fill entropy pool
	if(available < upperBound) {
		entropy = engine() - engine.min();
		available = full;
	}
	
	// Take entropy out
	uint dice = entropy % upperBound;
	entropy /= upperBound;
	available /= upperBound;
	return dice;
}

// true = white
bool blackOrWhite(uint numBlack, uint numWhite)
{
	// Short cut if there is no entropy involved
	if(numBlack == 0)
		return true;
	if(numWhite == 0)
		return false;
	
	// We can divide out gcd(numBlack, numWhite) , let's only do it for powers of two
	//uint ctz = trailingZeros(numBlack | numWhite);
	//numBlack >>= ctz;
	//numWhite >>= ctz;
	uint orred = numBlack | numWhite;
	while((orred & 1) == 0) {
		orred >>= 1;
		numBlack >>= 1;
		numWhite >>= 1;
	}
	
	return entropy(numBlack + numWhite) < numWhite;
}
