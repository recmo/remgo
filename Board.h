#pragma once
#include "Utilities.h"
#include "BoardMask.h"

class Board {
public:
	enum Player {
		White,
		Black,
		None
	};
	
	Board();
	~Board() { }
	bool operator==(const Board& other) const { return _moveCount == other._moveCount && _white == other._white && _black == other._black; }
	
	bool gameOver() const { return validMoves().empty(); }
	void playMove(Move move);
	
	vector<Move> validMoves(BoardPoint piece) const;
	vector<Move> validMoves() const;
	Move randomMove(BoardPoint piece) const;
	Move randomMove() const;
	
	uint moveCount() const { return _moveCount; }
	BoardMask white() const { return _white; }
	BoardMask black() const { return _black; }
	BoardMask playerPieces() const { return player() == White ? _white : _black; }
	BoardMask opponentPieces() const { return player() == White ? _black : _white; }
	BoardMask occupied() const { return _white | _black; }
	BoardMask free() const { return ~occupied(); }
	
	Player player() const { return (_moveCount & 1) ? Black : White; }
	Player winner() const { return gameOver() ? player() : None; }
	
protected:
	BoardMask _white;
	BoardMask _black;
	uint _moveCount;
};

std::ostream& operator<<(std::ostream& out, const Board& board);
