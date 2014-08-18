#include "Timer.h"

Timer Timer::instance(8, 53);

Timer::Timer(uint timeLimit, uint maxRounds)
: _timeLimit(timeLimit)
, _maxRounds(maxRounds)
, _roundLimit((_timeLimit * 1000000) / _maxRounds)
{
}

void Timer::nextRound()
{
	_roundStart = monotonic_clock::now();
}

bool Timer::ponder()
{
	monotonic_clock::time_point now = monotonic_clock::now();
	uint duration = duration_cast<microseconds>(now - _roundStart).count();
	return duration < _roundLimit;
}

