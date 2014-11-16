#include "Rotation.h"

//  r0 r1 r2 r3 mH mV dM dA
const uint8 Rotation::_multiplicationTable[Rotation::groupSize][Rotation::groupSize] = {
	{0, 1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 0, 6, 7, 5, 4},
	{2, 3, 0, 1, 5, 4, 7, 6},
	{3, 0, 1, 2, 7, 6, 4, 5},
	{4, 7, 5, 6, 0, 2, 3, 1},
	{5, 6, 4, 7, 2, 0, 1, 3},
	{6, 4, 7, 5, 1, 3, 0, 2},
	{7, 5, 6, 4, 3, 1, 2, 0},
};

std::ostream& operator<<(std::ostream& out, const Rotation& point)
{
	const char* strings[8] = {
		"R0", "R1", "R2", "R3", "MH", "MV", "DM", "DA"
	};
	out << strings[point._index];
	return out;
}
