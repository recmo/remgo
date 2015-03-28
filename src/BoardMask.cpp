#include "BoardMask.h"

wostream& operator<<(wostream& out, const BoardMask& mask)
{
	// ┌─┬─┐  250C 2500 252C 2500 2510
	// ├─┼─┤  251C 2500 253C 2500 2524
	// └─┴─┘  2514 2500 2534 2500 2518
	// ┌─█─┐
	// █─┼─┤  2588
	// └─┴─┘
	const wchar_t space = 0x2500;
	const wchar_t mark = 0x2588;
	const wchar_t box[3][3] = {
		{0x250C, 0x252C, 0x2510},
		{0x251C, 0x253C, 0x2524},
		{0x2514, 0x2534, 0x2518}
	};
	const int first = 0;
	const int last = BoardPoint::size - 1;
	for(int row = last; row >= first; --row) {
		const int rowbox = (row == first) ? 2 : ((row == last) ? 0 : 1);
		out << dec << setw(2) << setfill(L' ') << (row + 1);
		for(int col = first; col <= last; ++col) {
			const int colbox = (col == first) ? 0 : ((col == last) ? 2 : 1);
			if(mask.isSet(BoardPoint(row, col)))
				out << mark;
			else
				out << box[rowbox][colbox];
			if(col < last)
				out << space;
		}
		out << endl;
	}
	out << "  A B C D E F G H I J K L M N O P Q R S" << endl;
	return out;
}

const BoardMask BoardMask::fullBoard(createFullBoard());

BoardMask BoardMask::createFullBoard()
{
	BoardMask r;
	for(uint i = 0; i < BoardPoint::numPositions; ++i)
		r.set(BoardPoint(i));
	return r;
}

BoardMask::Iterator BoardMask::itterator() const
{
	return Iterator(*this);
}

BoardMask BoardMask::connected(const BoardMask& seed) const
{
	wcerr << "Unimplemented!" << endl;
	assert(false);
	return BoardMask();
}

vector<BoardMask> BoardMask::groups() const
{
	vector<BoardMask> result;
	BoardMask copy = *this;
	while(copy) {
		BoardMask group = copy.connected(BoardMask(copy.firstPoint()));
		result.push_back(group);
		copy -= group;
	}
	return result;
}

BoardPoint BoardMask::firstPoint() const
{
	wcerr << "Unimplemented!" << endl;
	assert(false);
	return BoardPoint();
}

BoardPoint BoardMask::randomPoint() const
{
	wcerr << "Unimplemented!" << endl;
	assert(false);
	return BoardPoint();
}
