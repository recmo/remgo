#pragma once
#include "utilities.h"
#include "BoardMask.h"

class Board {
public:
	enum Player {
		White,
		Black,
		None
	};
	
	Board(): _white(), _black(), _turn(0) { };
	Board(BoardMask white, BoardMask black, uint turn);
	~Board() { }
	bool operator==(const Board& other) const { return _turn == other._turn && _white == other._white && _black == other._black; }
	bool operator!=(const Board& other) const { return !operator==(other); }
	uint turn() const { return _turn; }
	Player player() const { return (_turn & 1) ? White : Black; }
	Player opponent() const { return (_turn & 1) ? Black : White; }
	BoardMask white() const { return _white; }
	BoardMask black() const { return _black; }
	BoardMask playerPieces() const { return player() == White ? _white : _black; }
	BoardMask opponentPieces() const { return player() == White ? _black : _white; }
	BoardMask occupied() const { return _white | _black; }
	BoardMask free() const { return ~occupied(); }
	BoardMask validMoves() const;
	bool isValidMove(BoardPoint move) const;
	bool gameOver() const { return validMoves().isEmpty(); }
	Board& play(BoardPoint move);
	
protected:
	BoardMask _white aligned;
	BoardMask _black aligned;
	uint _turn;
};

wostream& operator<<(wostream& out, const Board& board);
