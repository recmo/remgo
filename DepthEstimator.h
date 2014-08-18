#pragma once
#include "Utilities.h"

class DepthEstimator {
public:
	static DepthEstimator instance;
	DepthEstimator() : _count(0), _total(0) { }
	~DepthEstimator() { }
	
	void reset();
	void addEstimate(uint depth);
	float estimate() const;
	
protected:
	uint _count;
	uint _total;
};
