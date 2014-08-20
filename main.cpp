#include "Utilities.h"
#include "Random.h"
#include "Timer.h"
#include "BoardPoint.h"
#include "BoardMask.h"
#include "Move.h"
#include "Board.h"
#include "DepthEstimator.h"
#include "HeatMap.h"
#include "TreeNode.h"
#include "GameInputOutput.h"

/// TODO: Can we combine MCTS with the cellular automata supperoptimization?

void benchmark()
{
	// Benchmark
	const Board startingBoard;
	TreeNode root;
	uint simulations = 0;
	Timer::instance.update();
	float start = Timer::instance.remaining();
	while(Timer::instance.remaining() > 20.0) {
		root.rollOut(startingBoard);
		++simulations;
		Timer::instance.update();
	}
	float stop = Timer::instance.remaining();
	cerr << "Time: " << (start - stop) << endl;
	cerr << "Simulations: " << simulations << endl;
	cerr << "S/sec: " << (float(simulations)/(start - stop)) << endl;
}

int main(int argc, char* argv[])
{
	Timer::instance.start();
	cerr << "R " << argv[0]  << endl;
	cerr << "RAND_MAX = " << RAND_MAX << endl;
	cerr << "sizeof(float) = " << sizeof(float) << endl;
	cerr << "sizeof(uint) = " << sizeof(uint) << endl;
	cerr << "sizeof(uint128) = " << sizeof(uint128) << endl;
	cerr << "sizeof(void*) = " << sizeof(void*) << endl;
	cerr << "sizeof(BoardPoint) = " << sizeof(BoardPoint) << endl;
	cerr << "sizeof(Move) = " << sizeof(Move) << endl;
	cerr << "sizeof(BoardMask) = " << sizeof(BoardMask) << endl;
	cerr << "sizeof(Board) = " << sizeof(Board) << endl;
	cerr << "sizeof(TreeNode) = " << sizeof(TreeNode) << endl;
	srand(time(0));
	BoardMask::initialize();
	
	benchmark();
	
	GameInputOutput gio;
	gio.run();
	cerr << "Exit" << endl;
	Timer::instance.stop();
	return 0;
}
