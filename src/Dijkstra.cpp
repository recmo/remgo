#include "Dijkstra.h"

std::ostream& operator<<(std::ostream& out, const Dijkstra& dh)
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

Dijkstra::Dijkstra(const BoardMask& pieces, const BoardMask& free)
: _pieces(pieces)
, _free(free)
, _frontier()
, _frontierSize(0)
{
}

void Dijkstra::dijkstra()
{
	assert(_frontierSize == 0);
	BoardMask pieces = _pieces;
	
	// Initialize the board
	_minimalSpanningTree.clear();
	_minimalSpanningTreeRedudant.clear();
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
				addMST(front);
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

BoardPoint Dijkstra::bestVertex()
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

void Dijkstra::addVertex(BoardPoint p, uint distance)
{
	assert(_frontierSize < (BoardPoint::numPositions - 1));
	_frontier[_frontierSize++] = (distance << 8) | p.position();
}

void Dijkstra::addMST(BoardPoint p, BoardMask visited)
{
	// If we hit the MST we are done
	if(_minimalSpanningTree.isSet(p))
		return;
	
	// If we hit a start we are also done
	if(_distance[p.position()] == 0)
		return;
	
	// Mark visited
	assert(!visited.isSet(p));
	visited.set(p);
	assert(visited.isSet(p));

	// Find the lowest ranked neighbor
	BoardPoint path;
	uint distance = 0x100;
	uint count = 0;
	for(BoardPoint neighbor: p.neighbors()) {
		if(visited.isSet(neighbor))
			continue;
		if(_distance[neighbor.position()] < distance) {
			path = neighbor;
			distance = _distance[neighbor.position()];
			count = 1;
		} else if(_distance[neighbor.position()] == distance) {
			++count;
		}
	}
	if(count == 0) {
		cerr << visited << endl;
		cerr << _pieces << endl;
		cerr << _pieces.mask() << endl;
		cerr << _free << endl;
		cerr << _free.mask() << endl;
		cerr << *this << endl;
		assert(false);
		return;
	}
	
	if(count == 1) {
		// Unique path
		addMST(path, visited);
	} else {
		for(BoardPoint neighbor: p.neighbors()) {
			if(visited.isSet(neighbor))
				continue;
			// Multiple redundant paths
			if(_distance[neighbor.position()] == distance) {
				_minimalSpanningTreeRedudant.set(neighbor);
				addMST(neighbor, visited);
			}
		}
	}
	
	// Add to the MST
	_minimalSpanningTree.set(p);
}
