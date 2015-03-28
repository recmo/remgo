#pragma once
#include "Utilities.h"
#include "Board.h"
#include "BoardPoint.h"

class Dijkstra {
public:
	Dijkstra(const BoardMask& pieces, const BoardMask& free);
	
	void dijkstra();
	
	uint length() const { return _total; }
	BoardMask minimalSpanningTree() const { return _minimalSpanningTree; }
	BoardMask treeRedudancy() const { return _minimalSpanningTreeRedudant; }
	
private:
	friend std::ostream& operator<<(std::ostream& out, const Dijkstra& dh);
	
	BoardMask _pieces;
	BoardMask _free;
	uint8 _distance[BoardPoint::numPositions];
	BoardMask _minimalSpanningTree;
	BoardMask _minimalSpanningTreeRedudant;
	uint16 _frontier[BoardPoint::numPositions];
	uint _frontierSize;
	uint _total;
	
	bool frontierEmpty() { return _frontierSize == 0; }
	BoardPoint bestVertex();
	void addVertex(BoardPoint p, uint distance);
	void addMST(BoardPoint p, BoardMask visited = BoardMask());
};

std::ostream& operator<<(std::ostream& out, const Dijkstra& dh);
