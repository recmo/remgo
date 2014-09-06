#pragma once
#include "Utilities.h"
#include "Board.h"
#include "BoardPoint.h"

class DijkstraHeuristic {
public:
	DijkstraHeuristic(const BoardMask& pieces, const BoardMask& free) funk;
	
	uint dijkstra() funk;
	uint evalPosition() funk;
	uint evalMove(Move move) funk;
	
	BoardMask minimalSpanningTree() funk;
	
private:
	BoardMask _pieces;
	BoardMask _free;
	uint8 _distance[BoardPoint::numPositions];
	uint16 _frontier[BoardPoint::numPositions];
	uint _frontierSize;
	
	bool frontierEmpty() funk { return _frontierSize == 0; }
	BoardPoint bestVertex() funk;
	void addVertex(BoardPoint p, uint distance) funk;
};
