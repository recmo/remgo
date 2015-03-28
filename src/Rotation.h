#pragma once
#include "utilities.h"
#include <array>
class SymmetryGroup;

// Implements symmetry group D₄ (symmetries of the square) together with player-opponent turns
class Rotation {
public:
	static void test();
	
	static constexpr uint groupSize = 16;
	
	// Clockwise rotations
	static constexpr Rotation r0() { return Rotation(0); }
	static constexpr Rotation r1() { return Rotation(1); }
	static constexpr Rotation r2() { return Rotation(2); }
	static constexpr Rotation r3() { return Rotation(3); }
	
	// Horizontal and vertical mirror
	static constexpr Rotation mH() { return Rotation(4); }
	static constexpr Rotation mV() { return Rotation(5); }
	
	// Main diagonal and anti-diagonal mirror
	static constexpr Rotation dM() { return Rotation(6); }
	static constexpr Rotation dA() { return Rotation(7); }
	
	// Change piece color
	static constexpr Rotation pC() { return Rotation(8); }
	
	static const std::array<Rotation, Rotation::groupSize> all;
	
	constexpr Rotation() : _index(0) { }
	constexpr Rotation(uint8 index) : _index(index) { }
	
	bool operator==(const Rotation& other) const { return _index == other._index; }
	bool operator!=(const Rotation& other) const { return !operator==(other); }
	bool operator<(const Rotation& other) const { return _index < other._index; }
	Rotation& operator=(const Rotation& other) { _index = other._index; return *this; }
	Rotation operator*(const Rotation& other) const { return Rotation(_multiplicationTable[_index][other._index]); }
	Rotation& operator*=(const Rotation& other) { return operator=(operator*(other)); }
	Rotation operator/(const Rotation& other) const { return operator*(other.inverted());  }
	Rotation& operator/=(const Rotation& other) { return operator=(operator*(other.inverted())); }
	template<class T> T operator()(const T& value);
	
	uint index() const { return _index; }
	
	Rotation& invert() { return operator=(inverted()); }
	Rotation inverted() const { return Rotation(_inverse[_index]); }
	
	constexpr bool parityFlipped() const { return (_index & _d4) >= 4; }
	constexpr bool colourFlipped() const { return (_index & _colour); }
	
	void transform(uint size, uint& row, uint& col) const;
	template<class T> void permuteCorners(T& tl, T& tr, T& bl, T& br) const;
	
	uint64 hash() const { return _zobrist[_index]; }
	
private:
	friend wostream& operator<<(wostream& out, const Rotation& rotation);
	friend class SymmetryGroup;
	
	static constexpr uint8 _d4 = 0x7;
	static constexpr uint8 _colour = 0x8;
	static const uint8 _inverse[groupSize];
	static const uint8 _multiplicationTable[groupSize][groupSize];
	static const uint64 _zobrist[groupSize];
	uint8 _index;
};

wostream& operator<<(wostream& out, const Rotation& rotation);

inline void Rotation::transform(uint s, uint& row, uint& col) const
{
	--s;
	uint r = row;
	uint c = col;
	switch(_index & _d4) {
		case 0:
			return;
		case 1:
			row = c;
			col = s - r;
			return;
		case 2:
			row = s - r;
			col = s - c;
			return;
		case 3:
			row = s - c;
			col = r;
			return;
		case 4:
			row = s - r;
			return;
		case 5:
			col = s - c;
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

template<class T> inline void Rotation::permuteCorners(T& tl, T& tr, T& bl, T& br) const
{
	switch(_index & _d4) {
		case 0:
			return;
		case 1:
			swap(tl, br);
			swap(tl, bl);
			swap(tr, br);
			return;
		case 2:
			swap(tl, br);
			swap(bl, tr);
			return;
		case 3:
			swap(tl, br);
			swap(tl, tr);
			swap(bl, br);
			return;
		case 4:
			swap(tl, bl);
			swap(tr, br);
			return;
		case 5:
			swap(tl, tr);
			swap(bl, br);
			return;
		case 6:
			swap(bl, tr);
			return;
		case 7:
			swap(tl, br);
			return;
	}
}

template<> inline Rotation Rotation::operator()(const Rotation& value)
{
	return *this * value;
}
