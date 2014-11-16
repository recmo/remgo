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
//		"id", "r1", "r2", "r3", "fh", "fv", "fd", "fc"
	};
	out << strings[point._index];
	return out;
}

void Rotation::test()
{
	for(Rotation r: Rotation::all) {
		// Identity element
		assert(r * Rotation::r0() == r);
		assert(Rotation::r0() * r == r);
		
		// Inverse
		assert(r * r.inverted() == Rotation::r0());
		assert(r.inverted() * r == Rotation::r0());
	}
	for(Rotation a: Rotation::all)
	for(Rotation b: Rotation::all)
	for(Rotation c: Rotation::all) {
		// Associativity
		assert(a * (b * c) == (a * b) * c);
	}
	
	cerr << "    ";
	for(Rotation r: Rotation::all)
		cerr << r << "  ";
	cerr << endl;
	for(Rotation r1: Rotation::all) {
		cerr << r1 << "  ";
		for(Rotation r2: Rotation::all) {
			cerr << (r1 * r2) << "  ";
		}
		cerr << endl;
	}
	
	for(Rotation r: Rotation::all) {
		uint corners[4] = {1,2,3,4};
		r.permuteCorners(corners[0], corners[1], corners[2], corners[3]);
		cerr << r << endl;
		cerr << "  " << corners[0];
		cerr << " " << corners[1] << endl;
		cerr << "  " << corners[2];
		cerr << " " << corners[3] << endl;
		
		for(uint i = 0; i < 2; ++i) {
			for(uint j = 0; j < 2; ++j) {
				cerr << "  (" << i << ", " << j << ") |-> (";
				uint k = i;
				uint l = j;
				r.transform(2, k, l);
				cerr << k << ", " << l << ")" << endl;
			}
		}
	}
}
