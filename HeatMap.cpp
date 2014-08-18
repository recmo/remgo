#include "HeatMap.h"
#include "Move.h"

HeatMap HeatMap::white;
HeatMap HeatMap::black;

ostream& operator<<(ostream& out, const HeatMap& heatmap)
{
	for(uint i = 0; i < Move::maxIndex; ++i)
		out << " " << (float(heatmap._map[i]) / heatmap._max);
	return out;
}

HeatMap::HeatMap()
: _max(0)
, _map()
{
	for(uint i = 0; i < Move::maxIndex; ++i)
		_map[i] = 0;
}

void HeatMap::add(BoardMask moves)
{
	++_max;
	for(BoardMask::Iterator i = moves.itterator(); i; ++i)
		++_map[i->index()];
}

void HeatMap::scale(uint factor)
{
	_max /= factor;
	for(uint i = 0; i < Move::maxIndex; ++i)
		_map[i] /= factor;
}

Move HeatMap::bestMove(BoardMask moves) const
{
	uint bestScore = 0;
	Move bestMove;
	for(BoardMask::Iterator i = moves.itterator(); i; ++i) {
		const uint moveScore = _map[i->index()];
		if(moveScore <= bestScore)
			continue;
		bestScore = moveScore;
		bestMove = *i;
	}
	return bestMove;
}

float HeatMap::score(Move move) const
{
	if(_max == 0)
		return 0.5;
	return float(_map[move.index()]) / float(_max);
}

