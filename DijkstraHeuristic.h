#pragma once
#include "Utilities.h"
#include "Board.h"
#include "BoardPoint.h"

class DijkstraHeuristic {
public:
	DijkstraHeuristic(const BoardMask& pieces, const BoardMask& free) funk;
	
	void dijkstra() funk;
	uint evalPosition() funk { return _total; }
	sint evalMove(Move move) funk;
	
private:
	friend std::ostream& operator<<(std::ostream& out, const DijkstraHeuristic& dh) funk;
	
	BoardMask _pieces;
	BoardMask _free;
	uint8 _distance[BoardPoint::numPositions];
	BoardMask _minimalSpanningTree;
	uint16 _frontier[BoardPoint::numPositions];
	uint _frontierSize;
	uint _total;
	
	void updateVertex(BoardPoint p) funk;
	void propagateDistance(BoardPoint p, uint distance) funk;
	bool frontierEmpty() funk { return _frontierSize == 0; }
	BoardPoint bestVertex() funk;
	void addVertex(BoardPoint p, uint distance) funk;
};

std::ostream& operator<<(std::ostream& out, const DijkstraHeuristic& dh) funk;
