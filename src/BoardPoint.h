#pragma once
#include "Utilities.h"
#include "Rotation.h"

class BoardPoint {
public:
	static constexpr uint16 numPositions = 19 * 19;
	
	BoardPoint(): _position(numPositions) { }
	BoardPoint(uint8 position): _position(position) { }
	BoardPoint(uint8 row, uint8 col): _position(col * 11 + row) { }
	~BoardPoint() { }
	
	BoardPoint& operator=(const BoardPoint& other) { _position = other._position; return *this; }
	bool operator!=(const BoardPoint& other) const { return _position != other._position; }
	bool operator==(const BoardPoint& other) const { return _position == other._position; }
	bool operator<(const BoardPoint& other) const { return _position < other._position; }
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
	
	BoardPoint rotated(Rotation rotation) const;
	BoardPoint& rotate(Rotation rotation) { return operator=(rotated(rotation)); }
	
protected:
	uint16 _position; /// [1...361] inclusive
};

std::ostream& operator<<(std::ostream& out, const BoardPoint& position);

std::istream& operator>>(std::istream& in, BoardPoint& position);


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
