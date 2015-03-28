#pragma once
#include "Rotation.h"

// Implements subgroups of D₄ (symmetries of the square)
class SymmetryGroup
{
public:
	constexpr SymmetryGroup() : _index(0) { }
	explicit SymmetryGroup(uint16 mask);
	
	static SymmetryGroup trivial() { return SymmetryGroup(0x0001); }
	static SymmetryGroup colour() { return SymmetryGroup(0x0101); }
	static SymmetryGroup space() { return SymmetryGroup(0x00ff); }
	static SymmetryGroup all() { return SymmetryGroup(0xffff); }
	
	Rotation normalize(Rotation r) const { return Rotation(_normalized[_index][r._index]); }
	
private:
	static constexpr uint _numGroups = 27;
	static const uint16 _masks[_numGroups];
	static const uint8 _normalized[_numGroups][Rotation::groupSize];
	uint8 _index;
	
	Rotation normalizeCalculate(Rotation r) const;
};
