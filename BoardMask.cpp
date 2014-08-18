#include "BoardMask.h"
#include "Random.h"

std::ostream& operator<<(std::ostream& out, const BoardMask& mask)
{
	for(uint i = 1; i <= 106; ++i)
		out << (mask.isSet(i) ? "#" : ".");
	return out;
}

const BoardMask BoardMask::fullBoard(0xfffffffffffffffe, 0x7ffffffffff);

const BoardMask BoardMask::borders[5] {
	BoardMask().set(1).set(4).set(9).set(16).set(25).set(36).set(49),
	BoardMask().set(49).set(61).set(72).set(82).set(91).set(99).set(106),
	BoardMask().set(106).set(105).set(104).set(103).set(102).set(101).set(100),
	BoardMask().set(100).set(92).set(83).set(73).set(62).set(50).set(37),
	BoardMask().set(37).set(26).set(17).set(10).set(5).set(2).set(1)
};

BoardMask::BoardMask(BoardPoint point)
: _a(point.position() <= 63 ? (1ULL << point.position()) : 0)
, _b(point.position() <= 63 ? 0 : (1ULL << (point.position() - 64)))
{
}

BoardMask::Iterator BoardMask::itterator() const
{
	return Iterator(*this);
}

BoardMask BoardMask::expanded() const
{
	BoardMask result(*this);
	for(Iterator i(*this); i; ++i)
		result |= i->neighbors();
	return result;
}

BoardMask BoardMask::connected(const BoardMask& seed) const
{
	assert(isValid() && seed.isValid());
	BoardMask result = *this & seed;
	BoardMask border = result;
	while(border) {
		BoardMask nextBorder;
		for(auto i = border.itterator(); i; ++i)
			nextBorder |= i->neighbors() & *this;
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

uint BoardMask::controlledCorners() const
{
	uint corners = 0;
	
	// Iterate connected groups
	BoardMask remaining(*this);
	while(remaining) {
		// Find a group
		BoardMask group(remaining.firstPoint());
		group = remaining.connected(group);
		remaining -= group;
		
		// See which borders are connected
		uint borders = 0;
		for(uint i = 0; i < 5; ++i)
			if(group & BoardMask::borders[i])
				borders |= 1 << i;
		
		// If three borders are connected, any adjacent borders have the corner controlled
		if(::popcount(borders) >= 3)
			corners |= borders & ((borders >> 1) | (borders << 4));
	}
	return ::popcount(corners);
}

BoardPoint BoardMask::firstPoint() const
{
	if(_a)
		return BoardPoint(trailingZeros(_a));
	else if(_b)
		return BoardPoint(64 + trailingZeros(_b));
	else
		return BoardPoint();
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

BoardMask BoardMask::winningSet() const
{
	uint corners = 0;
	BoardMask result;
	
	// Iterate connected groups
	BoardMask remaining(*this);
	while(remaining) {
		// Find a group
		BoardMask group(remaining.firstPoint());
		group = remaining.connected(group);
		remaining -= group;
		
		// See which borders are connected
		uint borders = 0;
		for(uint i = 0; i < 5; ++i)
			if(group & BoardMask::borders[i])
				borders |= 1 << i;
		
		// If three borders are connected, any adjacent borders have the corner controlled
		if(::popcount(borders) >= 3) {
			uint groupCorners = borders & ((borders >> 1) | (borders << 4));
			if(groupCorners) {
				corners |= groupCorners;
				result |= group;
			}
		}
	}
	if(::popcount(corners) >= 3)
		return result;
	else
		return BoardMask();
}
