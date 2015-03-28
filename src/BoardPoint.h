#pragma once
#include "Utilities.h"
#include "Rotation.h"

class BoardPoint {
public:
	static constexpr uint8 numPositions = 11 * 11;
	
	BoardPoint() funk: _position(numPositions) { }
	BoardPoint(uint8 position) funk: _position(position) { }
	BoardPoint(uint8 row, uint8 col) funk: _position(col * 11 + row) { }
	~BoardPoint() funk { }
	
	BoardPoint& operator=(const BoardPoint& other) funk { _position = other._position; return *this; }
	bool operator!=(const BoardPoint& other) const funk { return _position != other._position; }
	bool operator==(const BoardPoint& other) const funk { return _position == other._position; }
	bool operator<(const BoardPoint& other) const funk { return _position < other._position; }
	bool isValid() const funk { return _position < numPositions; }
	uint8 row() const funk { return _position % 11; }
	uint8 col() const funk { return _position / 11; }
	uint8 position() const funk { return _position; }
	BoardPoint& position(uint8 value) funk { _position = value; return *this; }
	BoardPoint& row(uint8 r) funk { _position = col() * 11 + r; return *this; }
	BoardPoint& col(uint8 c) funk { _position = c * 11 + row(); return *this; }
	BoardPoint left() const funk { return row() > 0 ? BoardPoint(row() - 1, col()) : BoardPoint(); }
	BoardPoint right() const funk { return row() < 10 ? BoardPoint(row() + 1, col()) : BoardPoint(); }
	BoardPoint up() const funk { return col() > 0 ? BoardPoint(row(), col() - 1) : BoardPoint(); }
	BoardPoint down() const funk { return col() < 10 ? BoardPoint(row(), col() + 1) : BoardPoint(); }
	BoardMask mask() const funk;
	BoardMask neighbors() const funk;
	
	BoardPoint rotated(Rotation rotation) const funk;
	BoardPoint& rotate(Rotation rotation) funk { return operator=(rotated(rotation)); }
	
protected:
	uint8 _position; /// [1...106] inclusive
};

std::ostream& operator<<(std::ostream& out, const BoardPoint& position) funk;

std::istream& operator>>(std::istream& in, BoardPoint& position) funk;


inline BoardPoint BoardPoint::rotated(Rotation rotation) const
{
	uint r = row();
	uint c = col();
	rotation.transform(11, r, c);
	return BoardPoint(r, c);
}

template<> inline BoardPoint Rotation::operator()(const BoardPoint& value)
{
	return value.rotated(*this);
}
