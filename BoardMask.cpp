#include "BoardMask.h"
#include "Random.h"
#include "Board.h"

BoardMask BoardMask::masks[BoardPoint::numPositions + 1] __attribute__ ((aligned (16)));

void BoardMask::initialize()
{
	BoardMask mask(const128(0,1));
	for(uint i = 0; i < BoardPoint::numPositions; ++i) {
		masks[i] = mask;
		mask._mask <<= 1;
	}
	mask.clear();
	masks[BoardPoint::numPositions] = mask;
}

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
: _mask(BoardMask::masks[point.position()]._mask)
{
}

BoardMask::Iterator BoardMask::itterator() const
{
	return Iterator(*this);
}

BoardMask BoardMask::connected(const BoardMask& seed) const
{
	// Flanks, all non border positions
	const uint128 right = _mask & const128(0x1ffffffffffffffUL, 0xfffffffffffff800UL);
	const uint128 left  = _mask & const128(0x0003fffffffffffUL, 0xffffffffffffffffUL);
	const uint128 upper = _mask & const128(0x1ffbff7feffdffbUL, 0xff7feffdffbff7feUL);
	const uint128 lower = _mask & const128(0x0ffdffbff7feffdUL, 0xffbff7feffdffbffUL);
	uint128 r = seed.mask();
	uint128 oldr = r;
	do {
		oldr = r;
		r |= (r & lower) << 1;
		r |= (r & upper) >> 1;
		r |= (r & right) >> 11;
		r |= (r & left ) << 11;
		r &= _mask;
	} while(r != oldr);
	return BoardMask(r);
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
	uint n = popcount();
	if(n == 0)
		return BoardPoint();
	uint i = entropy(n);
	uint j = 0;
	for(BoardPoint p: *this) {
		if(i == j++)
			return p;
	}
	return BoardPoint();
}
