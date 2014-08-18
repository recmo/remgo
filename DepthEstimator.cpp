#include "DepthEstimator.h"

DepthEstimator DepthEstimator::instance;

void DepthEstimator::reset()
{
	_count = 0;
	_total = 0;
}

void DepthEstimator::addEstimate(uint depth)
{
	++_count;
	_total += depth;
}

float DepthEstimator::estimate() const
{
	return float(_total) / float(_count);
}
