#pragma once
#include "Utilities.h"
#include "BoardMask.h"

class Board {
public:
	Board(): _white(), _black(), _moveCount(0) { }
	~Board() { }
	bool operator==(const Board& other) const { return _moveCount == other._moveCount && _white == other._white && _black == other._black; }
	
	BoardMask nonSwapMoves() const { return ~(_white | _black); }
	bool gameOver() const;
	void playMove(Move move);
	
	uint moveCount() const { return _moveCount; }
	BoardMask white() const { return _white; }
	BoardMask black() const { return _black; }
	
	uint player() const { return (_moveCount & 1) ? 2 : 1; }
	uint winner() const;
	
	void bambooBridges();
	void randomFillUp();
	
protected:
	BoardMask _white;
	BoardMask _black;
	uint8 _moveCount;
};

std::ostream& operator<<(std::ostream& out, const Board& board);
