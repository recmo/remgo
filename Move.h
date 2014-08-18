#pragma once
#include "BoardPoint.h"

class Move {
public:
	Move(): _from(), _to() { }
	Move(BoardPoint from, BoardPoint to): _from(from), _to(to) { }
	
	bool operator==(const Move& other) const { return _from == other._from && _to == other._to; }
	bool operator!=(const Move& other) const { return !operator==(other); }
	
	BoardPoint from() const { return _from; }
	BoardPoint to() const { return _to; }
	Move& from(BoardPoint value) { _from = value; return *this; }
	Move& to(BoardPoint value) { _to = value; return *this; }
	
	bool isValid() const { return _from.isValid() && _to.isValid(); }
	
protected:
	BoardPoint _from;
	BoardPoint _to;
};

std::ostream& operator<<(std::ostream& out, const Move& move);

std::istream& operator>>(std::istream& in, Move& move);
