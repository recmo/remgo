#pragma once
#include "Utilities.h"
#include <chrono>
using namespace std::chrono;

/// TODO: Time distribution per round

class Timer {
public:
	static Timer instance;
	Timer(uint timeLimit, uint maxRounds);
	~Timer() { }
	
	void nextRound();
	bool ponder();
	
protected:
	uint _timeLimit;
	uint _maxRounds;
	uint _roundLimit;
	monotonic_clock::time_point _roundStart;
};
