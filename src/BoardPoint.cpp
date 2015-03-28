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
