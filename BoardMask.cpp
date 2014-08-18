#include "BoardMask.h"
#include "Random.h"

std::ostream& operator<<(std::ostream& out, const BoardMask& mask)
{
	for(uint i = 1; i <= 106; ++i)
		out << (mask.isSet(i) ? "#" : ".");
	return out;
}

const BoardMask BoardPoint::_neighbors[106] = {
	BoardMask().set(2).set(3).set(4), // 1
	BoardMask().set(1).set(3).set(5).set(6), // 2
	BoardMask().set(1).set(2).set(6).set(7).set(8).set(4), // 3
	BoardMask().set(1).set(3).set(8).set(9), // 4
	BoardMask().set(2).set(6).set(11).set(10), // 5
	BoardMask().set(2).set(5).set(11).set(12).set(7).set(3), // 6
	BoardMask().set(3).set(6).set(12).set(13).set(14).set(8), // 7
	BoardMask().set(4).set(3).set(7).set(14).set(15).set(9), // 8
	BoardMask().set(4).set(8).set(15).set(16), // 9
	BoardMask().set(5).set(11).set(18).set(17), // 10
	BoardMask().set(5).set(10).set(18).set(19).set(12).set(6), // 11
	BoardMask().set(6).set(11).set(19).set(20).set(13).set(7), // 12
	BoardMask().set(7).set(12).set(20).set(21).set(22).set(14), // 13
	BoardMask().set(8).set(7).set(13).set(22).set(23).set(15), // 14
	BoardMask().set(9).set(8).set(14).set(23).set(24).set(16), // 15
	BoardMask().set(9).set(15).set(24).set(25), // 16
	BoardMask().set(10).set(18).set(26).set(27), // 17
	BoardMask().set(10).set(17).set(27).set(28).set(19).set(11), // 18
	BoardMask().set(11).set(18).set(28).set(29).set(20).set(12), // 19
	BoardMask().set(12).set(19).set(29).set(30).set(21).set(13), // 20
	BoardMask().set(13).set(20).set(30).set(31).set(32).set(22), // 21
	BoardMask().set(14).set(13).set(21).set(32).set(33).set(23), // 22
	BoardMask().set(15).set(14).set(22).set(33).set(34).set(24), // 23
	BoardMask().set(16).set(15).set(23).set(34).set(35).set(25), // 24
	BoardMask().set(16).set(24).set(35).set(36), // 25
	BoardMask().set(17).set(27).set(37).set(38), // 26
	BoardMask().set(17).set(26).set(38).set(39).set(28).set(18), // 27
	BoardMask().set(18).set(27).set(39).set(40).set(29).set(19), // 28
	BoardMask().set(19).set(28).set(40).set(41).set(30).set(20), // 29
	BoardMask().set(20).set(29).set(41).set(42).set(31).set(21), // 30
	BoardMask().set(21).set(30).set(42).set(43).set(44).set(32), // 31
	BoardMask().set(22).set(21).set(31).set(44).set(45).set(33), // 32
	BoardMask().set(23).set(22).set(32).set(45).set(46).set(34), // 33
	BoardMask().set(24).set(23).set(33).set(46).set(47).set(35), // 34
	BoardMask().set(25).set(24).set(34).set(47).set(48).set(36), // 35
	BoardMask().set(25).set(35).set(48).set(49), // 36
	BoardMask().set(26).set(38).set(50), // 37
	BoardMask().set(26).set(37).set(50).set(51).set(39).set(27), // 38
	BoardMask().set(27).set(38).set(51).set(52).set(40).set(28), // 39
	BoardMask().set(28).set(39).set(52).set(53).set(41).set(29), // 40
	BoardMask().set(29).set(40).set(53).set(54).set(42).set(30), // 41
	BoardMask().set(30).set(41).set(54).set(55).set(43).set(31), // 42
	BoardMask().set(31).set(42).set(55).set(56).set(44), // 43
	BoardMask().set(31).set(43).set(56).set(57).set(45).set(32), // 44
	BoardMask().set(32).set(44).set(57).set(58).set(46).set(33), // 45
	BoardMask().set(33).set(45).set(58).set(59).set(47).set(34), // 46
	BoardMask().set(34).set(46).set(59).set(60).set(48).set(35), // 47
	BoardMask().set(35).set(47).set(60).set(61).set(49).set(36), // 48
	BoardMask().set(36).set(48).set(61), // 49
	BoardMask().set(37).set(38).set(51).set(62), // 50
	BoardMask().set(38).set(50).set(62).set(63).set(52).set(39), // 51
	BoardMask().set(39).set(51).set(63).set(64).set(53).set(40), // 52
	BoardMask().set(40).set(52).set(64).set(65).set(54).set(41), // 53
	BoardMask().set(41).set(53).set(65).set(66).set(55).set(42), // 54
	BoardMask().set(42).set(54).set(66).set(67).set(56).set(43), // 55
	BoardMask().set(43).set(55).set(67).set(68).set(57).set(44), // 56
	BoardMask().set(44).set(56).set(68).set(69).set(58).set(45), // 57
	BoardMask().set(45).set(57).set(69).set(70).set(59).set(46), // 58
	BoardMask().set(46).set(58).set(70).set(71).set(60).set(47), // 59
	BoardMask().set(47).set(59).set(71).set(72).set(61).set(48), // 60
	BoardMask().set(49).set(48).set(60).set(72), // 61
	BoardMask().set(50).set(51).set(63).set(73), // 62
	BoardMask().set(51).set(62).set(73).set(74).set(64).set(52), // 63
	BoardMask().set(52).set(63).set(74).set(75).set(65).set(53), // 64
	BoardMask().set(53).set(64).set(75).set(76).set(66).set(54), // 65
	BoardMask().set(54).set(65).set(76).set(77).set(67).set(55), // 66
	BoardMask().set(55).set(66).set(77).set(78).set(68).set(56), // 67
	BoardMask().set(56).set(67).set(78).set(79).set(69).set(57), // 68
	BoardMask().set(57).set(68).set(79).set(80).set(70).set(58), // 69
	BoardMask().set(58).set(69).set(80).set(81).set(71).set(59), // 70
	BoardMask().set(59).set(70).set(81).set(82).set(72).set(60), // 71
	BoardMask().set(61).set(60).set(71).set(82), // 72
	BoardMask().set(62).set(63).set(74).set(83), // 73
	BoardMask().set(63).set(73).set(83).set(84).set(75).set(64), // 74
	BoardMask().set(64).set(74).set(84).set(85).set(76).set(65), // 75
	BoardMask().set(65).set(75).set(85).set(86).set(77).set(66), // 76
	BoardMask().set(66).set(76).set(86).set(87).set(78).set(67), // 77
	BoardMask().set(67).set(77).set(87).set(88).set(79).set(68), // 78
	BoardMask().set(68).set(78).set(88).set(89).set(80).set(69), // 79
	BoardMask().set(69).set(79).set(89).set(90).set(81).set(70), // 80
	BoardMask().set(70).set(80).set(90).set(91).set(82).set(71), // 81
	BoardMask().set(72).set(71).set(81).set(91), // 82
	BoardMask().set(73).set(74).set(84).set(92), // 83
	BoardMask().set(74).set(83).set(92).set(93).set(85).set(75), // 84
	BoardMask().set(75).set(84).set(93).set(94).set(86).set(76), // 85
	BoardMask().set(76).set(85).set(94).set(95).set(87).set(77), // 86
	BoardMask().set(77).set(86).set(95).set(96).set(88).set(78), // 87
	BoardMask().set(78).set(87).set(96).set(97).set(89).set(79), // 88
	BoardMask().set(79).set(88).set(97).set(98).set(90).set(80), // 89
	BoardMask().set(80).set(89).set(98).set(99).set(91).set(81), // 90
	BoardMask().set(82).set(81).set(90).set(99), // 91
	BoardMask().set(83).set(84).set(93).set(100), // 92
	BoardMask().set(84).set(92).set(100).set(101).set(94).set(85), // 93
	BoardMask().set(85).set(93).set(101).set(102).set(95).set(86), // 94
	BoardMask().set(86).set(94).set(102).set(103).set(96).set(87), // 95
	BoardMask().set(87).set(95).set(103).set(104).set(97).set(88), // 96
	BoardMask().set(88).set(96).set(104).set(105).set(98).set(89), // 97
	BoardMask().set(89).set(97).set(105).set(106).set(99).set(90), // 98
	BoardMask().set(91).set(90).set(98).set(106), // 99
	BoardMask().set(92).set(93).set(101), // 100
	BoardMask().set(100).set(93).set(94).set(102), // 101
	BoardMask().set(101).set(94).set(95).set(103), // 102
	BoardMask().set(102).set(95).set(96).set(104), // 103
	BoardMask().set(103).set(96).set(97).set(105), // 104
	BoardMask().set(104).set(97).set(98).set(106), // 105
	BoardMask().set(105).set(98).set(99) // 106
};

const BoardMask BoardMask::fullBoard(0xfffffffffffffffe, 0x7ffffffffff);

const BoardMask BoardMask::borders[5] {
	BoardMask().set(1).set(4).set(9).set(16).set(25).set(36).set(49),
	BoardMask().set(49).set(61).set(72).set(82).set(91).set(99).set(106),
	BoardMask().set(106).set(105).set(104).set(103).set(102).set(101).set(100),
	BoardMask().set(100).set(92).set(83).set(73).set(62).set(50).set(37),
	BoardMask().set(37).set(26).set(17).set(10).set(5).set(2).set(1)
};

BoardMask BoardPoint::mask() const
{
	return BoardMask(*this);
}

BoardMask BoardPoint::neighbors() const
{
	assert(isValid());
	return _neighbors[position() - 1];
}

BoardMask::BoardMask(BoardPoint point)
: _a(point.position() <= 63 ? (1ULL << point.position()) : 0)
, _b(point.position() <= 63 ? 0 : (1ULL << (point.position() - 64)))
{
}

BoardMask::Iterator BoardMask::itterator() const
{
	return Iterator(*this);
}

BoardMask BoardMask::expanded() const
{
	BoardMask result(*this);
	for(Iterator i(*this); i; ++i)
		result |= i->neighbors();
	return result;
}

BoardMask BoardMask::connected(const BoardMask& seed) const
{
	assert(isValid() && seed.isValid());
	BoardMask result = *this & seed;
	BoardMask border = result;
	while(border) {
		BoardMask nextBorder;
		for(auto i = border.itterator(); i; ++i)
			nextBorder |= i->neighbors() & *this;
		nextBorder -= result;
		result |= nextBorder;
		border = nextBorder;
	}
	return result;
}

vector<BoardMask> BoardMask::groups() const
{
	vector<BoardMask> result;
	BoardMask copy = *this;
	while(copy) {
		BoardMask group = copy.connected(copy.firstPoint());
		result.push_back(group);
		copy -= group;
	}
	return result;
}

uint BoardMask::controlledCorners() const
{
	uint corners = 0;
	
	// Iterate connected groups
	BoardMask remaining(*this);
	while(remaining) {
		// Find a group
		BoardMask group(remaining.firstPoint());
		group = remaining.connected(group);
		remaining -= group;
		
		// See which borders are connected
		uint borders = 0;
		for(uint i = 0; i < 5; ++i)
			if(group & BoardMask::borders[i])
				borders |= 1 << i;
		
		// If three borders are connected, any adjacent borders have the corner controlled
		if(::popcount(borders) >= 3)
			corners |= borders & ((borders >> 1) | (borders << 4));
	}
	return ::popcount(corners);
}

BoardPoint BoardMask::firstPoint() const
{
	if(_a)
		return BoardPoint(trailingZeros(_a));
	else if(_b)
		return BoardPoint(64 + trailingZeros(_b));
	else
		return BoardPoint();
}

BoardPoint BoardMask::randomPoint() const
{
	uint p = popcount();
	if(p == 0)
		return BoardPoint();
	uint n = entropy(p);
	Iterator i(*this);
	while(n--)
		i++;
	return *i;
}

BoardMask BoardMask::winningSet() const
{
	uint corners = 0;
	BoardMask result;
	
	// Iterate connected groups
	BoardMask remaining(*this);
	while(remaining) {
		// Find a group
		BoardMask group(remaining.firstPoint());
		group = remaining.connected(group);
		remaining -= group;
		
		// See which borders are connected
		uint borders = 0;
		for(uint i = 0; i < 5; ++i)
			if(group & BoardMask::borders[i])
				borders |= 1 << i;
		
		// If three borders are connected, any adjacent borders have the corner controlled
		if(::popcount(borders) >= 3) {
			uint groupCorners = borders & ((borders >> 1) | (borders << 4));
			if(groupCorners) {
				corners |= groupCorners;
				result |= group;
			}
		}
	}
	if(::popcount(corners) >= 3)
		return result;
	else
		return BoardMask();
}
