#include "SymmetryGroup.h"

Rotation SymmetryGroup::normalize(Rotation r) const
{
	uint16 mask = _mask;
	Rotation lowest = r;
	for(Rotation a: Rotation::all) {
		if(mask & 1) {
			Rotation b = r * a;
			if(b < lowest)
				lowest = b;
		}
		mask >>= 1;
	}
	return lowest;
}
