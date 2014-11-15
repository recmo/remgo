#include "BoardPoint.h"
#include "BoardMask.h"

std::ostream& operator<<(std::ostream& out, const BoardPoint& point)
{
	if(!point.isValid()) {
		out << "00";
		return out;
	}
	out << static_cast<char>('A' + point.col());
	out << point.row() + 1;
	return out;
}

std::istream& operator>>(std::istream& in, BoardPoint& point)
{
	char c;
	in >> c;
	point.col(c - 'A');
	uint r;
	in >> r;
	point.row(r - 1);
	assert(point.isValid());
	return in;
}

BoardMask BoardPoint::mask() const
{
	return BoardMask(*this);
}

BoardMask BoardPoint::neighbors() const
{
	BoardMask result(*this);
	result.expand();
	result.clear(*this);
	return result;
}

BoardPoint BoardPoint::rotated(uint rotation) const
{
	/// @todo Lookup table?
	bool flipped = rotation >= 4;
	rotation %= 4;
	uint r = row();
	uint c = col();
	while(rotation--) {
		uint t = 11 - c;
		c = r;
		r = t;
	}
	if(flipped) {
		uint t = c;
		c = r;
		r = t;
	}
	return BoardPoint(r, c);
}

