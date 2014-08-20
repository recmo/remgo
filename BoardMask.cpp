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
	
	// Flanks, all non border positions
	uint128 right = const128(0x1ffffffffffffffUL, 0xfffffffffffff800UL);
	uint128 left  = const128(0x0003fffffffffffUL, 0xffffffffffffffffUL);
	uint128 upper = const128(0x1ffbff7feffdffbUL, 0xff7feffdffbff7feUL);
	uint128 lower = const128(0x0ffdffbff7feffdUL, 0xffbff7feffdffbffUL);
	
	// right masks everything that can go right
	upper = _mask & ((_mask & upper) >> 1); 
	lower = _mask & ((_mask & lower) >> 1); 
	right = _mask & ((_mask & right) >> 11);
	left  = _mask & ((_mask & left ) << 11);
	
	uint128 r = seed.mask() & _mask;
	uint128 oldr = r;
	do {
		oldr = r;
		r |= (r & lower) << 1;
		r |= (r & upper) >> 1;
		r |= (r & right) >> 11;
		r |= (r & left ) << 11;
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
