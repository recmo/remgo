#pragma once
#include "Utilities.h"

class BoardPoint {
public:
	static constexpr uint maxRotation = 10;
	static constexpr uint maxIndex = 105;
	static constexpr uint numIndices = 106;
	static BoardPoint fromIndex(uint index) { return BoardPoint(index + 1); }
	
	BoardPoint(): _position(0) { }
	BoardPoint(uint position): _position(position) { }
	~BoardPoint() { }
	
	uint index() const { return _position - 1; }
	bool operator!=(const BoardPoint& other) const { return _position != other._position; }
	bool operator==(const BoardPoint& other) const { return _position == other._position; }
	bool isValid() const { return _position >= 1 && _position <= 106; }
	uint position() const { return _position; }
	BoardMask mask() const;
	BoardPoint& rotate(uint degrees) { *this = rotated(degrees); return *this; }
	BoardPoint rotated(uint degrees) const { return BoardPoint(_rotations[degrees][index()]); }
	BoardMask neighbors() const;
	BoardPoint& position(uint value) { _position = value; return *this; }
	
protected:
	static const BoardMask _neighbors[106];
	static const uint8 _rotations[10][106];
	uint _position; /// [1...106] inclusive
};

