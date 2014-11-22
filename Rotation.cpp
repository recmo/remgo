#include "Rotation.h"


const std::array<Rotation, Rotation::groupSize> Rotation::all =  std::array<Rotation, Rotation::groupSize>{{
	Rotation(0), Rotation(1), Rotation(2), Rotation(3),
	Rotation(4), Rotation(5), Rotation(6), Rotation(7),
	Rotation(8), Rotation(9), Rotation(10), Rotation(11),
	Rotation(12), Rotation(13), Rotation(14), Rotation(15),
}};

const uint8 Rotation::_inverse[Rotation::groupSize] = {
	0, 3, 2, 1, 4, 5, 6, 7, 8, 11, 10, 9, 12, 13, 14, 15
};

const uint8 Rotation::_multiplicationTable[Rotation::groupSize][Rotation::groupSize] = {
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
	{ 1,  2,  3,  0,  6,  7,  5,  4,  9, 10, 11,  8, 14, 15, 13, 12},
	{ 2,  3,  0,  1,  5,  4,  7,  6, 10, 11,  8,  9, 13, 12, 15, 14},
	{ 3,  0,  1,  2,  7,  6,  4,  5, 11,  8,  9, 10, 15, 14, 12, 13},
	{ 4,  7,  5,  6,  0,  2,  3,  1, 12, 15, 13, 14,  8, 10, 11,  9},
	{ 5,  6,  4,  7,  2,  0,  1,  3, 13, 14, 12, 15, 10,  8,  9, 11},
	{ 6,  4,  7,  5,  1,  3,  0,  2, 14, 12, 15, 13,  9, 11,  8, 10},
	{ 7,  5,  6,  4,  3,  1,  2,  0, 15, 13, 14, 12, 11,  9, 10,  8},
	{ 8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7},
	{ 9, 10, 11,  8, 14, 15, 13, 12,  1,  2,  3,  0,  6,  7,  5,  4},
	{10, 11,  8,  9, 13, 12, 15, 14,  2,  3,  0,  1,  5,  4,  7,  6},
	{11,  8,  9, 10, 15, 14, 12, 13,  3,  0,  1,  2,  7,  6,  4,  5},
	{12, 15, 13, 14,  8, 10, 11,  9,  4,  7,  5,  6,  0,  2,  3,  1},
	{13, 14, 12, 15, 10,  8,  9, 11,  5,  6,  4,  7,  2,  0,  1,  3},
	{14, 12, 15, 13,  9, 11,  8, 10,  6,  4,  7,  5,  1,  3,  0,  2},
	{15, 13, 14, 12, 11,  9, 10,  8,  7,  5,  6,  4,  3,  1,  2,  0}
};

const uint64 Rotation::_zobrist[Rotation::groupSize] = {
	0xcebb05b80b78146f,
	0x9ad9b5dec5ba2de1,
	0xbbec2f29ce39c845,
	0xf80eef26c1cae2c9,
	0x91a56459cea34abf,
	0xe7c509ccded1f5a9,
	0xef9bcac560202a29,
	0xedf1b11585d2d773,
	0xc3419fce45e0f60d,
	0xa5a30ac342a3ca8b,
	0xa68cc54abff07291,
	0xbac5a330f432a163,
	0xc49b0708c1768cbb,
	0x8b2bdd9f8da3f489,
	0x91d9da056ed6dc85,
	0xee96a1fa54e4745f
};

std::ostream& operator<<(std::ostream& out, const Rotation& point)
{
	const char* strings[Rotation::groupSize] = {
		"R0N", "R1N", "R2N", "R3N", "MHN", "MVN", "DMN", "DAN",
		"R0C", "R1C", "R2C", "R3C", "MHC", "MVC", "DMC", "DAC"
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
