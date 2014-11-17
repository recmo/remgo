#pragma once
#include "Rotation.h"

// Implements subgroups of D₄ (symmetries of the square)
class SymmetryGroup
{
public:
	constexpr SymmetryGroup() : _index(0) { }
	
	static constexpr SymmetryGroup trivial() { return SymmetryGroup(0); }
	static constexpr SymmetryGroup space() { return SymmetryGroup(10); }
	static constexpr SymmetryGroup all() { return SymmetryGroup(20); }
	
	Rotation normalize(Rotation r) const funk;
	bool equals(Rotation a, Rotation b) const funk;
	
private:
	constexpr SymmetryGroup(uint index) : _index(index) { }
	uint8 _index;
};

inline Rotation SymmetryGroup::normalize(Rotation r) const
{
	switch(_index) {
		case 0:
			return r;
		/// TODO
		case 10: {
			if(r.colourFlipped())
				return Rotation::pC();
			else
				return Rotation();
		}
		case 20: {
			return Rotation();
		}
	};
	assert(false);
}
