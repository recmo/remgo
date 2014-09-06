#include "DijkstraHeuristic.h"

std::ostream& operator<<(std::ostream& out, const DijkstraHeuristic& dh)
{
	// Print
	for(uint row = 0; row < 11; ++row) {
		for(uint col = 0; col < 11; ++col) {
			BoardPoint i(10 - row, col);
			if(dh._distance[i.position()] == 0xff)
				out << ".";
			else
				out << uint(dh._distance[i.position()]);
			if(dh._pieces.isSet(i))
				out << "] ";
			else
				out << "  ";
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
				path = 0;
				
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

uint DijkstraHeuristic::evalMove(Move move)
{
	const bool playerMove = _pieces.isSet(move.from());
	const bool fromMst = _minimalSpanningTree.isSet(move.from());
	const bool toMst = _minimalSpanningTree.isSet(move.to());
	assert(!_free.isSet(move.from()));
	assert(_free.isSet(move.to()));
	
	if(playerMove) {
		if(fromMst && !toMst)
			return -1;
		if(!fromMst && toMst)
			return 1;
	} else {
		if(toMst) {
			// Check for component split?
			return -1;
		}
	}
	return 0;
}

void DijkstraHeuristic::updateVertex(BoardPoint p)
{
	const uint weigth = 0xff;
	if(_free.isSet(p))
		weigth = 1;
	else if(_pieces.isSet(p))
		weigth = 0;
	
	// Update the vertex itself
	
	// Propagate the update to its neighbors
}

void DijkstraHeuristic::propagateDistance(BoardPoint p, uint distance)
{
	assert(frontierEmpty());
	if(_distance[p.position()] == distance)
		return;
	
	// Traversable places
	const BoardMask traversable = _pieces | _free;
	
	// Set the distance and start the frontier
	_distance[p.position()] = distance;
	addVertex(p, distance);
	
	// Propagate in Dijkstra fashion
	while(!frontierEmpty()) {
		const BoardPoint front = bestVertex();
		uint distance = _distance[front.position()];
		
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
				path = 0;
			}
			if(path < _distance[neighbor.position()]) {
				_distance[neighbor.position()] = path;
				addVertex(neighbor, path);
			}
		}
	}
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
