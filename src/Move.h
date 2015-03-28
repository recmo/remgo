#pragma once
#include "BoardPoint.h"

class Move {
public:
	Move(): _from(), _to() { }
	Move(BoardPoint from, BoardPoint to): _from(from), _to(to) { }
	
	Move& operator=(const Move& other) { _from = other._from; _to = other._to; return *this; }
	bool operator==(const Move& other) const { return _from == other._from && _to == other._to; }
	bool operator!=(const Move& other) const { return !operator==(other); }
	bool operator<(const Move& other) const;
	
	BoardPoint from() const { return _from; }
	BoardPoint to() const { return _to; }
	Move& from(BoardPoint value) { _from = value; return *this; }
	Move& to(BoardPoint value) { _to = value; return *this; }
	
	bool isValid() const { return _from.isValid() && _to.isValid(); }
	
	Move rotated(Rotation rotation) const { return Move(_from.rotated(rotation), _to.rotated(rotation)); }
	Move& rotate(Rotation rotation) { return operator=(rotated(rotation)); }
	
protected:
	BoardPoint _from;
	BoardPoint _to;
};

std::ostream& operator<<(std::ostream& out, const Move& move);

std::istream& operator>>(std::istream& in, Move& move);

inline bool Move::operator<(const Move& other) const
{
	if(_from != other._from)
		return _from < other._from;
	return _to < other._to;
}

template<> inline Move Rotation::operator()(const Move& value)
{
	return value.rotated(*this);
}
