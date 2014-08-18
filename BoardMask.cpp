#include "BoardMask.h"
#include "Random.h"

std::ostream& operator<<(std::ostream& out, const BoardMask& mask)
{
	for(BoardPoint point: BoardMask::fullBoard)
		out << (mask.isSet(point) ? "#" : ".");
	return out;
}

const BoardMask BoardMask::fullBoard = BoardMask().setFullBoard();

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

BoardMask BoardMask::expanded() const
{
	BoardMask result(*this);
	for(BoardPoint p: *this)
		result |= p.neighbors();
	return result;
}

BoardMask BoardMask::connected(const BoardMask& seed) const
{
	assert(isValid() && seed.isValid());
	BoardMask result = *this & seed;
	BoardMask border = result;
	while(border) {
		BoardMask nextBorder;
		for(BoardPoint p: border)
			nextBorder |= p.neighbors() & *this;
		nextBorder -= result;
		result |= nextBorder;
		border = nextBorder;
	}
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
	uint p = popcount();
	if(p == 0)
		return BoardPoint();
	uint n = entropy(p);
	Iterator i(*this);
	while(n--)
		i++;
	return *i;
}
