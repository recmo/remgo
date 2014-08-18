#include "Utilities.h"
#include "BoardMask.h"
#include "Move.h"

class HeatMap {
public:
	static HeatMap white;
	static HeatMap black;
	
	HeatMap();
	~HeatMap() { }
	
	void add(BoardMask points);
	void scale(uint factor);
	BoardPoint bestPoint(BoardMask mask) const;
	float score(BoardPoint point) const;
	
protected:
	friend ostream& operator<<(ostream& out, const HeatMap& heatmap);
	uint _max;
	uint _map[BoardPoint::numPositions];
};

ostream& operator<<(ostream& out, const HeatMap& heatmap);
