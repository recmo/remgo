#include "DijkstraHeuristic.h"


DijkstraHeuristic::DijkstraHeuristic(const BoardMask& pieces, const BoardMask& free)
: _pieces(pieces)
, _free(free)
, _frontier()
, _frontierSize(0)
{
}

uint DijkstraHeuristic::dijkstra()
{
	BoardMask pieces = _pieces;
	
	// Initialize the board
	for(uint i = 0; i < BoardPoint::numPositions; ++i)
		_distance[i] = 0xff;
	
	// Initialize the frontier
	addVertex(pieces.firstPoint(), 0);
	_distance[pieces.firstPoint().position()] = 0;
	pieces.clear(pieces.firstPoint());
	
	// Dijkstra
	const BoardMask traversable = _pieces | _free;
	uint total = 0;
	uint piecesDiscovered = 1;
	while(!frontierEmpty()) {
		const BoardPoint front = bestVertex();
		uint distance = _distance[front.position()];
		
		// For each direction
		for(BoardPoint neighbor: front.neighbors() & traversable) {
			if(_distance[neighbor.position()] != 0xff)
				continue;
			uint8 path = distance;
			bool piecePlace = false;
			if(_free.isSet(neighbor))
				++path;
			else {
				++piecesDiscovered;
				pieces.clear(neighbor);
				total += path;
				path = 0;
			}
			if(path < _distance[neighbor.position()]) {
				_distance[neighbor.position()] = path;
				addVertex(neighbor, path);
			}
		}
		
		if(frontierEmpty() && piecesDiscovered < 30) {
			// Pick a new frontier
			addVertex(pieces.firstPoint(), 0);
			_distance[pieces.firstPoint().position()] = 0;
			pieces.clear(pieces.firstPoint());
		}
	}
	
	/// TODO: Board may be disconnected. Keep track of number of pieces encounter -> constant
	assert(piecesDiscovered == 30);
	
	return total;
	
	// Print
	for(uint row = 0; row < 11; ++row) {
		for(uint col = 0; col < 11; ++col) {
			BoardPoint i(10 - row, col);
			if(_distance[i.position()] == 0xff)
				cerr << " ";
			else
				cerr << uint(_distance[i.position()]);
			if(_pieces.isSet(i))
				cerr << "] ";
			else
				cerr << "  ";
		}
		cerr << endl;
	}
	cerr << total << endl;
	cerr << endl;
}

BoardPoint DijkstraHeuristic::bestVertex()
{
	uint32 best = 0xffffffff;
	uint bestIndex = 0;
	for(uint i = 0; i < _frontierSize; ++i) {
		if(_frontier[i] < best) {
			best = _frontier[i];
			bestIndex = i;
		}
	}
	swap(_frontier[bestIndex], _frontier[_frontierSize - 1]);
	--_frontierSize;
	return BoardPoint(best & 0xff);
}

void DijkstraHeuristic::addVertex(BoardPoint p, uint distance)
{
	_frontier[_frontierSize++] = (distance << 8) | p.position();
}
