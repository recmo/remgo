#include "SymmetryGroup.h"

uint8 SymmetryGroup::_normalized[SymmetryGroup::_numGroups][Rotation::groupSize];

const uint16 SymmetryGroup::_masks[SymmetryGroup::_numGroups] = {
	0x0001,
	0x0005,
	0x0011,
	0x0021,
	0x0035,
	0x0041,
	0x0081,
	0x00c5,
	0x00ff,
	0x0101,
	0x0401,
	0x1001,
	0x1111,
	0x1421,
	0x2001,
	0x2121,
	0x2411,
	0x3005,
	0x3ac5,
	0x4001,
	0x4141,
	0x4481,
	0x8001,
	0x8181,
	0x8441,
	0xc5c5,
	0xffff
};

void SymmetryGroup::initialize()
{
	for(uint index = 0; index < _numGroups; ++index) {
		for(Rotation r: Rotation::all) {
			SymmetryGroup sg(_masks[index]);
			_normalized[index][r._index] = sg.normalizeCalculate(r)._index;
		}
	}
}


SymmetryGroup::SymmetryGroup(uint16 mask)
: _index(0)
{
	for(; _index < _numGroups; ++_index)
		if(_masks[_index] == mask)
			break;
	assert(_index != _numGroups);
}


Rotation SymmetryGroup::normalizeCalculate(Rotation r) const
{
	uint16 mask = _masks[_index];
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
