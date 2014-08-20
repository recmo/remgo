#include "BoardMask.h"
#include "Random.h"

std::ostream& operator<<(std::ostream& out, const BoardMask& mask)
{
	for(uint row = 0; row < 11; ++row) {
		for(uint col = 0; col < 11; ++col) {
			BoardPoint i(10 - row, col);
			out << (mask.isSet(i) ? "#" : ".");
		}
		out << endl;
	}
	return out;
}

const BoardMask BoardMask::fullBoard(const128(0x1ffffffffffffffUL, 0xffffffffffffffffUL));

BoardMask& BoardMask::setFullBoard()
{
	for(uint i = 0; i < BoardPoint::numPositions; ++i)
		set(BoardPoint(i));
	return *this;
}

BoardMask::BoardMask(BoardPoint point)
: _mask(uint128(1) << point.position())
{
}

BoardMask::Iterator BoardMask::itterator() const
{
	return Iterator(*this);
}

BoardMask BoardMask::connected(const BoardMask& seed) const
{
	assert(isValid() && seed.isValid());
	BoardMask result = *this & seed;
	BoardMask oldResult;
	/// TODO: We can unroll expanded and precompose our mask with the four masks
	do {
		oldResult = result;
		result.expand();
		result &= *this;
	} while(oldResult != result);
	return result;
}

vector<BoardMask> BoardMask::groups() const
{
	vector<BoardMask> result;
	BoardMask copy = *this;
	while(copy) {
		BoardMask group = copy.connected(copy.firstPoint());
		result.push_back(group);
		copy -= group;
	}
	return result;
}

BoardPoint BoardMask::firstPoint() const
{
	if(isEmpty())
		return BoardPoint();
	else
		return BoardPoint(trailingZeros(_mask));
}

BoardPoint BoardMask::randomPoint() const
{
	/// TODO: Optimize
	uint p = popcount();
	if(p == 0)
		return BoardPoint();
	uint n = entropy(p);
	Iterator i(*this);
	while(n--)
		i++;
	return *i;
}
