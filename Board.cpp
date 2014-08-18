#include "Board.h"
#include "Move.h"
#include "Random.h"

std::ostream& operator<<(std::ostream& out, const Board& board)
{
	for(uint i = 1; i <= 106; ++i) {
		bool white = board.white().isSet(i);
		bool black = board.black().isSet(i);
		if(!white && !black)
			out << ".";
		if(white && !black)
			out << "W";
		if(!white && black)
			out << "B";
		if(white && black)
			out << "!";
	}
	return out;
}

void Board::playMove(Move move)
{
	// Increase move counter
	++_moveCount;
	
	// Swap move
	if(move == Move::Swap) {
		assert(_moveCount == 2);
		swap(_black, _white);
		return;
	}
	
	// Placement move
	assert(!_black.isSet(move));
	assert(!_white.isSet(move));
	if(_moveCount & 1) {
		_black.set(move);
	} else {
		_white.set(move);
	}
}

bool Board::gameOver() const
{
	return false;
}

uint Board::winner() const
{
	return 0;
}

// Connects bamboo bridges randomly
void Board::bambooBridges()
{
}

void Board::randomFillUp()
{
}
