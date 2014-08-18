#pragma once
#include "Utilities.h"

class BoardPoint {
public:
	static constexpr uint8 numPositions = 11 * 11;
	
	BoardPoint(): _position(numPositions) { }
	BoardPoint(uint8 position): _position(position) { }
	BoardPoint(uint8 row, uint8 col): _position(col * 11 + row) { }
	~BoardPoint() { }
	
	bool operator!=(const BoardPoint& other) const { return _position != other._position; }
	bool operator==(const BoardPoint& other) const { return _position == other._position; }
	bool isValid() const { return _position < numPositions; }
	uint8 row() const { return _position % 11; }
	uint8 col() const { return _position / 11; }
	uint8 position() const { return _position; }
	BoardPoint& position(uint8 value) { _position = value; return *this; }
	BoardPoint& row(uint8 r) { _position = col() * 11 + r; return *this; }
	BoardPoint& col(uint8 c) { _position = c * 11 + row(); return *this; }
	BoardPoint left() const { return row() > 0 ? BoardPoint(row() - 1, col()) : BoardPoint(); }
	BoardPoint right() const { return row() < 10 ? BoardPoint(row() + 1, col()) : BoardPoint(); }
	BoardPoint up() const { return col() > 0 ? BoardPoint(row(), col() - 1) : BoardPoint(); }
	BoardPoint down() const { return col() < 10 ? BoardPoint(row(), col() + 1) : BoardPoint(); }
	BoardMask mask() const;
	BoardMask neighbors() const;
	
protected:
	uint8 _position; /// [1...106] inclusive
};

std::ostream& operator<<(std::ostream& out, const BoardPoint& position);

std::istream& operator>>(std::istream& in, BoardPoint& position);
