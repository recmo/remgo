#include "DijkstraHeuristic.h"

/*

The MST should be those board place (free or occupied) that need to be filled with player
pieces such that all the players pieces form one unit.

This means that anything but the leafs are included.

The from location of a move can be either of two cases:
* Removing MST piece.
* Removing non-MST piece.

The to location can be either of four cases:
* Filling MST vacancy and blocking opponent MST vacancy.
* Filling own MST vacancy.
* Blocking opponent vacancy.
* Placing as non-MST piece.

*/

std::ostream& operator<<(std::ostream& out, const DijkstraHeuristic& dh)
{
	// Print
	for(uint row = 0; row < 11; ++row) {
		for(uint col = 0; col < 11; ++col) {
			BoardPoint i(10 - row, col);
			if(dh._distance[i.position()] == 0xff)
				out << "#";
			else if(dh._pieces.isSet(i))
				out << char('A' + uint(dh._distance[i.position()]));
			else
				out << char('a' + uint(dh._distance[i.position()]));
		}
		out << endl;
	}
	out << dh._total << endl;
	out << endl;
	return out;
}

DijkstraHeuristic::DijkstraHeuristic(const BoardMask& pieces, const BoardMask& free)
: _pieces(pieces)
, _free(free)
, _frontier()
, _frontierSize(0)
{
}

void DijkstraHeuristic::dijkstra()
{
	assert(_frontierSize == 0);
	BoardMask pieces = _pieces;
	
	// Initialize the board
	_minimalSpanningTree.clear();
	for(uint i = 0; i < BoardPoint::numPositions; ++i)
		_distance[i] = 0xff;
	
	assert(pieces.popcount() == 30);
	
	// Initialize the frontier
	addVertex(pieces.firstPoint(), 0);
	_distance[pieces.firstPoint().position()] = 0;
	pieces.clear(pieces.firstPoint());
	
	// Dijkstra
	const BoardMask traversable = _pieces | _free;
	_total = 0;
	uint piecesDiscovered = 1;
	while(!frontierEmpty()) {
		const BoardPoint front = bestVertex();
		uint distance = _distance[front.position()];
		
		assert(front.isValid());
		
		// For each direction
		for(BoardPoint neighbor: front.neighbors() & traversable) {
			if(_distance[neighbor.position()] != 0xff)
				continue;
			uint8 path = distance;
			if(_free.isSet(neighbor))
				++path;
			else {
				++piecesDiscovered;
				pieces.clear(neighbor);
				_total += path;
				
				// A shortest path has been found, add it to the MST
			}
			if(path < _distance[neighbor.position()]) {
				_distance[neighbor.position()] = path;
				addVertex(neighbor, path);
			}
		}
		if(frontierEmpty() && piecesDiscovered < 30) {
			// We explored the component, but there are more pieces in another component.
			assert(!pieces.isEmpty());
			
			// Pick a new frontier
			addVertex(pieces.firstPoint(), 0);
			_distance[pieces.firstPoint().position()] = 0;
			pieces.clear(pieces.firstPoint());
			++piecesDiscovered;
		}
	}
	
	// Make sure we found all connected components
	assert(piecesDiscovered == 30);
}

BoardPoint DijkstraHeuristic::bestVertex()
{
	assert(_frontierSize < BoardPoint::numPositions);
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
	assert(_frontierSize < (BoardPoint::numPositions - 1));
	_frontier[_frontierSize++] = (distance << 8) | p.position();
}
