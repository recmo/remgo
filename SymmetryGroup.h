#pragma once
#include "Rotation.h"

// Implements subgroups of D₄ (symmetries of the square)
class SymmetryGroup
{
public:
	constexpr SymmetryGroup() : _mask(0) { }
	constexpr SymmetryGroup(Rotation r) : _mask(1UL << r._index) { }
	constexpr SymmetryGroup(uint16 mask) : _mask(mask) { }
	
	static constexpr SymmetryGroup trivial() { return SymmetryGroup(); }
	static constexpr SymmetryGroup colour() { return SymmetryGroup(Rotation::pC()); }
	static constexpr SymmetryGroup space() { return SymmetryGroup(0xff); }
	static constexpr SymmetryGroup all() { return SymmetryGroup(0xffff); }
	
	Rotation normalize(Rotation r) const funk;
	
	uint16 _mask;
private:
};
