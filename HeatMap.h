#include "Utilities.h"
#include "BoardMask.h"
#include "Move.h"

class HeatMap {
public:
	static HeatMap white;
	static HeatMap black;
	
	HeatMap();
	~HeatMap() { }
	
	void add(BoardMask moves);
	void scale(uint factor);
	Move bestMove(BoardMask moves) const;
	float score(Move move) const;
	
protected:
	friend ostream& operator<<(ostream& out, const HeatMap& heatmap);
	uint _max;
	uint _map[Move::maxIndex];
};

ostream& operator<<(ostream& out, const HeatMap& heatmap);
