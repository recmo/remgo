#pragma once
#include "BoardPoint.h"

class Move {
public:
	Move() funk: _from(), _to() { }
	Move(BoardPoint from, BoardPoint to) funk: _from(from), _to(to) { }
	
	Move& operator=(const Move& other) funk { _from = other._from; _to = other._to; return *this; }
	bool operator==(const Move& other) const funk { return _from == other._from && _to == other._to; }
	bool operator!=(const Move& other) const funk { return !operator==(other); }
	bool operator<(const Move& other) const funk;
	
	BoardPoint from() const funk { return _from; }
	BoardPoint to() const funk { return _to; }
	Move& from(BoardPoint value) funk { _from = value; return *this; }
	Move& to(BoardPoint value) funk { _to = value; return *this; }
	
	bool isValid() const funk { return _from.isValid() && _to.isValid(); }
	
	Move rotated(Rotation rotation) const funk { return Move(_from.rotated(rotation), _to.rotated(rotation)); }
	Move& rotate(Rotation rotation) funk { return operator=(rotated(rotation)); }
	
protected:
	BoardPoint _from;
	BoardPoint _to;
};

std::ostream& operator<<(std::ostream& out, const Move& move) funk;

std::istream& operator>>(std::istream& in, Move& move) funk;

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
