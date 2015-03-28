#pragma once
#include "Utilities.h"
#include "BoardMask.h"
#include "Move.h"

class Board {
public:
	enum Player {
		White,
		Black,
		None
	};
	
	Board();
	Board(BoardMask white, BoardMask black, uint moveCount);
	~Board() { }
	bool operator==(const Board& other) const { return _moveCount == other._moveCount && _white == other._white && _black == other._black; }
	bool operator!=(const Board& other) const { return !operator==(other); }
	
	bool gameOver() const { return validMoves().empty(); }
	Board& playMove(Move move);
	
	vector<Move> validMoves(BoardPoint piece) const;
	vector<Move> validMoves() const;
	bool isValidMove(Move move) const;
	vector<Move> sortedMoves() const;
	Move heuristicMove() const;
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
	Player opponent() const { return (_moveCount & 1) ? White : Black; }
	Player winner() const { return gameOver() ? player() : None; }
	
	sint heuristicStrength() const;
	
protected:
	BoardMask _white aligned;
	BoardMask _black aligned;
	uint _moveCount;
};

std::ostream& operator<<(std::ostream& out, const Board& board);
