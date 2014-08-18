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
	
	void start();
	void nextRound();
	bool ponder();
	void stop();
	
	float remaining() const { return _timeLimit / 1000000.0; }
	
protected:
	uint _timeLimit;
	uint _maxRounds;
	uint _roundLimit;
	monotonic_clock::time_point _start;
	monotonic_clock::time_point _roundStart;
};
