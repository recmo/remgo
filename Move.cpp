#include "Move.h"

std::ostream& operator<<(std::ostream& out, const Move& move)
{
	out << move.from() << "-" << move.to();
	return out;
}

std::istream& operator>>(std::istream& in, Move& move)
{
	BoardPoint p;
	in >> p;
	move.from(p);
	char c;
	in >> c;
	assert(c == '-');
	in >> p;
	move.to(p);
	return in;
}
