#pragma once
#include "Utilities.h"

// Implements symmetry group D₄ (symmetries of the square)
class Rotation {
public:
	static constexpr uint groupSize = 8;
	
	// Clockwise rotations
	static constexpr Rotation r0() { return Rotation(0); }
	static constexpr Rotation r1() { return Rotation(1); }
	static constexpr Rotation r2() { return Rotation(2); }
	static constexpr Rotation r3() { return Rotation(3); }
	
	// Horizontal and vertical mirror
	static const Rotation mH() { return Rotation(4); }
	static const Rotation mV() { return Rotation(5); }
	
	// Main diagonal and anti-diagonal mirror
	static const Rotation dM() { return Rotation(6); }
	static const Rotation dA() { return Rotation(7); }
	
	constexpr Rotation(uint8 index) funk : _index(index) { }
	
	Rotation operator*(const Rotation& other) const funk;
	template<class T> T operator()(const T& value) funk;
	
	constexpr bool flipped() const funk { return _index >= 4; }
	void transform(uint size, uint& row, uint& col) const funk;
	
private:
	friend std::ostream& operator<<(std::ostream& out, const Rotation& rotation) funk;
	
	static const uint8 _multiplicationTable[groupSize][groupSize];
	uint8 _index;
};

std::ostream& operator<<(std::ostream& out, const Rotation& rotation) funk;

inline void Rotation::transform(uint s, uint& row, uint& col) const
{
	--s;
	uint r = row;
	uint c = col;
	switch(_index) {
		case 0:
			return;
		case 1:
			row = s - c;
			col = r;
			return;
		case 2:
			row = s - r;
			col = s - c;
			return;
		case 3:
			row = c;
			col = s - r;
			return;
		case 4:
			col = s - c;
			return;
		case 5:
			row = s - r;
			return;
		case 6:
			row = c;
			col = r;
			return;
		case 7:
			row = s - c;
			col = s - r;
			return;
	}
}

inline Rotation Rotation::operator*(const Rotation& other) const
{
	return Rotation(_multiplicationTable[_index][other._index]);
}

template<> inline Rotation Rotation::operator()(const Rotation& value)
{
	return *this * value;
}
