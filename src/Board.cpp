#include "Board.h"

wostream& operator<<(wostream& out, const Board& board)
{
	// Unicode box drawing:
	// ┌─┬─┐  250C 2500 252C 2500 2510
	// ├─┼─┤  251C 2500 253C 2500 2524
	// └─┴─┘  2514 2500 2534 2500 2518
	// ┌─W─┐
	// B─╬─┤  256C
	// └─┴─▒
	const wchar_t space = 0x2500;
	const wchar_t hoshi = 0x256C;
	const wchar_t white = L'O'; // L' '; // L'W';
	const wchar_t black = L'X'; //L'B';
	const wchar_t error = L'?';
	const wchar_t box[3][3] = {
		{0x250C, 0x252C, 0x2510},
		{0x251C, 0x253C, 0x2524},
		{0x2514, 0x2534, 0x2518}
	};
	const int first = 0;
	const int last = BoardPoint::size - 1;
	for(int row = last; row >= first; --row) {
		const int rowbox = (row == first) ? 2 : ((row == last) ? 0 : 1);
		const bool rowhoshi = (row == 3 || row == 9 || row == 15);
		out << dec << setw(2) << setfill(L' ') << (row + 1) << " ";
		for(int col = first; col <= last; ++col) {
			const int colbox = (col == first) ? 0 : ((col == last) ? 2 : 1);
			const bool colhoshi = (col == 3 || col == 9 || col == 15);
			const BoardPoint pos(row, col);
			const bool isWhite = board.white().isSet(pos);
			const bool isBlack = board.black().isSet(pos);
			if(isWhite  && !isBlack)
				out << white;
			else if(!isWhite  && isBlack)
				out << black;
			else if(isWhite  && isBlack)
				out << error;
			else if (rowhoshi && colhoshi)
				out << hoshi;
			else
				out << box[rowbox][colbox];
			if(col < last)
				out << space;
		}
		out << endl;
	}
	out << "   A B C D E F G H J K L M N O P Q R S T" << endl;
	if(board.player() == Board::White)
		out << "White to play" << endl;
	else
		out << "Black to play" << endl;
	return out;
}

Board::Board(BoardMask white, BoardMask black, uint turn)
: _white(white)
, _black(black)
, _turn(turn)
{
}

BoardMask Board::validMoves() const
{
	BoardMask result = free();
	for(BoardPoint move: result) {
		if(!isValidMove(move))
			result.clear(move);
	}
	
	return result;
}

bool Board::isValidMove(BoardPoint move) const
{
	if(!free().isSet(move))
		return false;
	
	// Copy player state
	BoardMask f = free();
	BoardMask p = playerPieces();
	
	// Add the move
	p.set(move);
	f.clear(move);
	
	// Calculate player alive pieces
	BoardMask playerAlive = p.connected(f.expanded());
	
	// Disallow suicide
	if(!playerAlive.isSet(move))
		return false;
	
	// TODO: Ko
	// TODO: Super-Ko
	
	return true;
}

Board& Board::play(BoardPoint position)
{
	// Pass moves
	if(!position.isValid()) {
		++_turn;
		return *this;
	}
	
	// Verify the move correctness
	assert(position.isValid());
	assert(free().isSet(position));
	
	// Placement move. Capture stones.
	if(player() == White) {
		_white.set(position);
		_black = _black.connected(free().expanded());
		//_white = _white.connected(free().expanded());
	} else {
		_black.set(position);
		_white = _white.connected(free().expanded());
		//_black = _black.connected(free().expanded());
	}
	
	// Increase move counter
	++_turn;
	return *this;
}

sint Board::score()
{
	BoardMask f = free();
	BoardMask b = black();
	BoardMask w = white();
	
	// Remove dead groups
	bool fixedpoint = false;
	do {
		fixedpoint = true;
		for(BoardPoint i: b) {
			BoardMask group = b.connected(i);
			uint liberties = (group.expanded() & f).popcount();
			if(liberties < 2) {
				b -= group;
				f |= group;
				fixedpoint = false;
			}
		}
		for(BoardPoint i: w) {
			BoardMask group = w.connected(i);
			uint liberties = (group.expanded() & f).popcount();
			if(liberties < 2) {
				w -= group;
				f |= group;
				fixedpoint = false;
			}
		}
	} while(!fixedpoint);
	
	// Colour territories
	BoardMask ba = b;
	BoardMask wa = w;
	for(BoardPoint i: f) {
		BoardMask space = f.connected(i).expanded();
		bool touchesBlack = !(space & b).isEmpty();
		bool touchesWhite = !(space & w).isEmpty();
		if(touchesBlack && !touchesWhite)
			ba.set(i);
		if(!touchesBlack && touchesWhite)
			wa.set(i);
	}
	
	Board fin;
	fin._white = wa;
	fin._black = ba;
	return ba.popcount() - wa.popcount();
}
