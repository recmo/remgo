#include "Timer.h"
#include "DepthEstimator.h"

Timer Timer::instance(30, 100);

Timer::Timer(uint timeLimit, uint maxRounds)
: _timeLimit(timeLimit * 1000000)
, _maxRounds(maxRounds)
, _roundLimit((_timeLimit * 1000000) / _maxRounds)
{
}

void Timer::start()
{
	_start = monotonic_clock::now();
}

void Timer::stop()
{
	monotonic_clock::time_point now = monotonic_clock::now();
	uint duration = duration_cast<microseconds>(now - _start).count();
	_timeLimit -= duration;
}

void Timer::update()
{
	monotonic_clock::time_point now = monotonic_clock::now();
	uint duration = duration_cast<microseconds>(now - _start).count();
	_timeLimit -= duration;
	_start = now;
}

void Timer::nextRound()
{
	_roundStart = monotonic_clock::now();
	uint roundEstimate = DepthEstimator::instance.estimateRemaining() / 2.0;
	roundEstimate += 1;
	_roundLimit = _timeLimit / roundEstimate;
}

bool Timer::ponder()
{
	monotonic_clock::time_point now = monotonic_clock::now();
	uint duration = duration_cast<microseconds>(now - _roundStart).count();
	return duration < _roundLimit;
}

