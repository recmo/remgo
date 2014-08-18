#pragma once
#include "BoardPoint.h"

class Move: public BoardPoint {
public:
	static constexpr uint maxIndex = 106;
	static constexpr uint numIndices = 107;
	static Move fromIndex(uint index) { return Move(index + 1); }
	static Move Swap;
	
	Move(): BoardPoint(0) { }
	Move(const BoardPoint& point): BoardPoint(point) { }
	explicit Move(const string& str);
	Move(uint position): BoardPoint(position) { }
	~Move() { }
	
	bool isValid() const { return BoardPoint::isValid() || (*this == Swap); }
	
protected:
	static const BoardMask _neighbors[106];
};

Move Move::Swap(107);

std::ostream& operator<<(std::ostream& out, const Move& move)
{
	if(move == Move::Swap)
		out << "-1";
	else
		out << move.position();
	return out;
}

std::istream& operator>>(std::istream& in, Move& move)
{
	int position;
	in >> position;
	move = (position == -1) ? Move::Swap : Move(position);
	assert(move.isValid());
	return in;
}

Move::Move(const string& str)
: BoardPoint(0)
{
	stringstream stream(str);
	stream >> *this;
}
