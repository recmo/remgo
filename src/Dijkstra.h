#pragma once
#include "Utilities.h"
#include "Board.h"
#include "BoardPoint.h"

class Dijkstra {
public:
	Dijkstra(const BoardMask& pieces, const BoardMask& free) funk;
	
	void dijkstra() funk;
	
	uint length() const funk { return _total; }
	BoardMask minimalSpanningTree() const funk { return _minimalSpanningTree; }
	BoardMask treeRedudancy() const funk { return _minimalSpanningTreeRedudant; }
	
private:
	friend std::ostream& operator<<(std::ostream& out, const Dijkstra& dh) funk;
	
	BoardMask _pieces;
	BoardMask _free;
	uint8 _distance[BoardPoint::numPositions];
	BoardMask _minimalSpanningTree;
	BoardMask _minimalSpanningTreeRedudant;
	uint16 _frontier[BoardPoint::numPositions];
	uint _frontierSize;
	uint _total;
	
	bool frontierEmpty() funk { return _frontierSize == 0; }
	BoardPoint bestVertex() funk;
	void addVertex(BoardPoint p, uint distance) funk;
	void addMST(BoardPoint p, BoardMask visited = BoardMask()) funk;
};

std::ostream& operator<<(std::ostream& out, const Dijkstra& dh) funk;
