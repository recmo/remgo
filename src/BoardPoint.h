#pragma once
#include "utilities.h"
#include "Rotation.h"
class BoardMask;

class BoardPoint {
public:
	static constexpr uint16 size = 19;
	static constexpr uint16 numPositions = size * size;
	
	BoardPoint(): _position(numPositions) { }
	BoardPoint(uint16 position): _position(position) { }
	BoardPoint(uint16 row, uint16 col): _position(col * size + row) { }
	~BoardPoint() { }
	
	BoardPoint& operator=(const BoardPoint& other) { _position = other._position; return *this; }
	bool operator!=(const BoardPoint& other) const { return _position != other._position; }
	bool operator==(const BoardPoint& other) const { return _position == other._position; }
	bool operator<(const BoardPoint& other) const { return _position < other._position; }
	bool isValid() const { return _position < numPositions; }
	uint16 row() const { return _position % size; }
	uint16 col() const { return _position / size; }
	uint16 position() const { return _position; }
	BoardPoint& position(uint16 value) { _position = value; return *this; }
	BoardPoint& row(uint16 r) { _position = col() * size + r; return *this; }
	BoardPoint& col(uint16 c) { _position = c * size + row(); return *this; }
	BoardPoint left() const { return row() > 0 ? BoardPoint(row() - 1, col()) : BoardPoint(); }
	BoardPoint right() const { return row() < (size - 1) ? BoardPoint(row() + 1, col()) : BoardPoint(); }
	BoardPoint up() const { return col() > 0 ? BoardPoint(row(), col() - 1) : BoardPoint(); }
	BoardPoint down() const { return col() < (size - 1) ? BoardPoint(row(), col() + 1) : BoardPoint(); }
	BoardMask mask() const;
	BoardMask neighbors() const;
	
	BoardPoint rotated(Rotation rotation) const;
	BoardPoint& rotate(Rotation rotation) { return operator=(rotated(rotation)); }
	
protected:
	uint16 _position; /// [0...numPositions] inclusive
};

wostream& operator<<(wostream& out, const BoardPoint& position);

wistream& operator>>(wistream& in, BoardPoint& position);


inline BoardPoint BoardPoint::rotated(Rotation rotation) const
{
	uint r = row();
	uint c = col();
	rotation.transform(size, r, c);
	return BoardPoint(r, c);
}

template<> inline BoardPoint Rotation::operator()(const BoardPoint& value)
{
	return value.rotated(*this);
}
