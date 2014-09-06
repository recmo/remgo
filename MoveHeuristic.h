#pragma once
#include "Utilities.h"
#include "Board.h"
#include "BoardPoint.h"
#include "Dijkstra.h"

class MoveHeuristic {
public:
	MoveHeuristic(const Board& board);
	
	float evaluate(Move move);
	
private:
	static const float improveMSTScore;
	static const float nonLeafPieceScore;
	static const float opponentHinderScore;
	static const float opponentBlockScore;
	static const float freeNeighborScore;
	static const float opponentNeighborScore;
	
	const Board& _board;
	float _oldNeighborScore;
	BoardMask _playerMst;
	BoardMask _opponentMst;
	BoardMask _playerRedundant;
	BoardMask _opponentRedundant;
};
