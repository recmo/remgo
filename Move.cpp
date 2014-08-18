#include "Move.h"

Move Move::Swap(107);

std::ostream& operator<<(std::ostream& out, const Move& move)
{
	if(move == Move::Swap)
		out << "-1";
	else
		out << move.position();
	return out;
}

std::istream& operator>>(std::istream& in, Move& move)
{
	int position;
	in >> position;
	move = (position == -1) ? Move::Swap : Move(position);
	assert(move.isValid());
	return in;
}

Move::Move(const string& str)
: BoardPoint(0)
{
	stringstream stream(str);
	stream >> *this;
}
