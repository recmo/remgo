#include "Rotation.h"


const std::array<Rotation, 8> Rotation::all =  std::array<Rotation, 8>{{
	Rotation(0), Rotation(1), Rotation(2), Rotation(3),
	Rotation(4), Rotation(5), Rotation(6), Rotation(7)
}};

const uint8 Rotation::_inverse[Rotation::groupSize] = {
	0, 3, 2, 1, 4, 5, 6, 7
};

//  r0 r1 r2 r3 mH mV dM dA
const uint8 Rotation::_multiplicationTable[Rotation::groupSize][Rotation::groupSize] = {
	{0, 1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 0, 6, 7, 5, 4},
	{2, 3, 0, 1, 5, 4, 7, 6},
	{3, 0, 1, 2, 7, 6, 4, 5},
	{4, 7, 5, 6, 0, 2, 3, 1},
	{5, 6, 4, 7, 2, 0, 1, 3},
	{6, 4, 7, 5, 1, 3, 0, 2},
	{7, 5, 6, 4, 3, 1, 2, 0}
};

const uint64 Rotation::_zobrist[Rotation::groupSize] = {
	0xcebb05b80b78146f,
	0x9ad9b5dec5ba2de1,
	0xbbec2f29ce39c845,
	0xf80eef26c1cae2c9,
	0x91a56459cea34abf,
	0xe7c509ccded1f5a9,
	0xef9bcac560202a29,
	0xedf1b11585d2d773
};

std::ostream& operator<<(std::ostream& out, const Rotation& point)
{
	const char* strings[8] = {
		"R0", "R1", "R2", "R3", "MH", "MV", "DM", "DA"
	};
	out << strings[point._index];
	return out;
}
