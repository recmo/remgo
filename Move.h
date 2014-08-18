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

std::ostream& operator<<(std::ostream& out, const Move& move);

std::istream& operator>>(std::istream& in, Move& move);
