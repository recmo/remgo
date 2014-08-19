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
	
	Board() funk;
	~Board() funk { }
	bool operator==(const Board& other) const funk { return _moveCount == other._moveCount && _white == other._white && _black == other._black; }
	
	bool gameOver() const funk { return validMoves().empty(); }
	void playMove(Move move) funk;
	
	vector<Move> validMoves(BoardPoint piece) const funk;
	vector<Move> validMoves() const funk;
	Move randomMove(BoardPoint piece) const funk;
	Move randomMove() const funk;
	
	uint moveCount() const funk { return _moveCount; }
	BoardMask white() const funk { return _white; }
	BoardMask black() const funk { return _black; }
	BoardMask playerPieces() const funk { return player() == White ? _white : _black; }
	BoardMask opponentPieces() const funk { return player() == White ? _black : _white; }
	BoardMask occupied() const funk { return _white | _black; }
	BoardMask free() const funk { return ~occupied(); }
	
	Player player() const funk { return (_moveCount & 1) ? Black : White; }
	Player winner() const funk { return gameOver() ? player() : None; }
	
protected:
	BoardMask _white;
	BoardMask _black;
	uint _moveCount;
};

std::ostream& operator<<(std::ostream& out, const Board& board);
