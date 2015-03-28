#include "HeatMap.h"
#include "Move.h"

HeatMap HeatMap::white;
HeatMap HeatMap::black;

ostream& operator<<(ostream& out, const HeatMap& heatmap)
{
	for(uint i = 0; i < BoardPoint::numPositions; ++i)
		out << " " << (float(heatmap._map[i]) / heatmap._max);
	return out;
}

HeatMap::HeatMap()
: _max(0)
, _map()
{
	for(uint i = 0; i < BoardPoint::numPositions; ++i)
		_map[i] = 0;
}

void HeatMap::add(BoardMask points)
{
	++_max;
	for(BoardPoint p: points)
		++_map[p.position()];
}

void HeatMap::scale(uint factor)
{
	_max /= factor;
	for(uint i = 0; i < BoardPoint::numPositions; ++i)
		_map[i] /= factor;
}

BoardPoint HeatMap::bestPoint(BoardMask mask) const
{
	uint bestScore = 0;
	BoardPoint bestPoint;
	for(BoardPoint p: mask) {
		const uint moveScore = _map[p.position()];
		if(moveScore <= bestScore)
			continue;
		bestScore = moveScore;
		bestPoint = p;
	}
	return bestPoint;
}

float HeatMap::score(BoardPoint point) const
{
	if(_max == 0)
		return 0.5;
	return float(_map[point.position()]) / float(_max);
}

