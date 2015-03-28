#include "BoardPoint.h"
#include "BoardMask.h"

wostream& operator<<(wostream& out, const BoardPoint& point)
{
	if(!point.isValid()) {
		out << "000";
		return out;
	}
	// Skip the letter 'I'
	if(point.col() <= 7)
		out << static_cast<char>('A' + point.col());
	else
		out << static_cast<char>('A' + point.col() + 1);
	out << dec << setw(2) << setfill(L'0') << (point.row() + 1);
	return out;
}

wistream& operator>>(wistream& in, BoardPoint& point)
{
	wchar_t c;
	in >> c;
	// Skip the letter 'I'
	if(c > L'I')
		--c;
	point.col(c - L'A');
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
