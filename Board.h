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
	
	Board() funk;
	Board(BoardMask white, BoardMask black, uint moveCount) funk;
	~Board() funk { }
	bool operator==(const Board& other) const funk { return _moveCount == other._moveCount && _white == other._white && _black == other._black; }
	bool operator!=(const Board& other) const funk { return !operator==(other); }
	
	bool gameOver() const funk { return validMoves().empty(); }
	Board& playMove(Move move) funk;
	
	vector<Move> validMoves(BoardPoint piece) const funk;
	vector<Move> validMoves() const funk;
	bool isValidMove(Move move) const funk;
	vector<Move> sortedMoves() const funk;
	Move heuristicMove() const funk;
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
	Player opponent() const funk { return (_moveCount & 1) ? White : Black; }
	Player winner() const funk { return gameOver() ? player() : None; }
	
	sint heuristicStrength() const funk;
	
protected:
	BoardMask _white aligned;
	BoardMask _black aligned;
	uint _moveCount;
};

std::ostream& operator<<(std::ostream& out, const Board& board);
