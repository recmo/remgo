#include "GameInputOutput.h"
#include "HeatMap.h"
#include "DepthEstimator.h"
#include "Timer.h"

GameInputOutput::GameInputOutput()
: _board()
, _tree(new TreeNode())
, _current(_tree)
{
}

GameInputOutput::~GameInputOutput()
{
	delete _tree;
}

void GameInputOutput::run()
{
	cerr << "Game loop" << endl;
	string line;
	for(;;) {
		// Player move
		cin >> line;
		cerr << "In: " << line << endl;
		if(line == "Quit") {
			cerr << "Quiting" << endl;
			return;
		}
		if(line != "Start") {
			Move move(line);
			assert(move.isValid());
			playMove(move);
			if(_board.gameOver())
				break;
		}
		
		// Own move
		Move move = generateMove();
		playMove(move);
		cerr << _board << endl;
		cerr << "Out: " << move << endl;
		cout << move << endl;
		if(_board.gameOver())
			break;
	}
	cin >> line;
	cerr << "In: " << line << endl;
	cerr << "Quiting" << endl;
}

Move GameInputOutput::generateMove()
{
	cerr << "Thinking from ";
	cerr << TreeNode::numNodes()  << " nodes (" << _current->backwardVisits() << " visits)" << " (";
	cerr << (TreeNode::numNodes() * sizeof(TreeNode) / (1024*1024))  << " MB)" << endl;
	assert(_current);
	
	// Scale HeatMap down so new entries are valued more
	HeatMap::black.scale(10);
	HeatMap::white.scale(10);
	
	// Reset depth estimator
	DepthEstimator::instance.reset();
	
	// Iterate MCTS a couple of times
	Timer::instance.nextRound();
	while(Timer::instance.ponder()) {
		for(uint i = 0; i < 100; ++i)
			_current->selectAction(_board);
	}
	
	// cerr << "White heatmap " << HeatMap::white << endl;
	// cerr << "Black heatmap " << HeatMap::black << endl;
	cerr << "Current depth " << _board.moveCount() << endl;
	cerr << "Estimated total depth " << DepthEstimator::instance.estimate() << endl;
	
	cerr << "Thought to ";
	cerr << TreeNode::numNodes()  << " nodes (" << _current->backwardVisits() << " visits)" << " (";
	cerr << (TreeNode::numNodes() * sizeof(TreeNode) / (1024*1024))  << " MB)" << endl;
	return _current->bestMove();
}

void GameInputOutput::playMove(Move move)
{
	_board.playMove(move);
	TreeNode* vincent = _current->child(move);	
	_current->vincent(vincent);
	_current = vincent;
	cerr << "Playing " << move << " ";
	cerr << TreeNode::numNodes()  << " nodes (" << _current->backwardVisits() << " visits)" << " (";
	cerr << (TreeNode::numNodes() * sizeof(TreeNode) / (1024*1024))  << " MB)" << endl;
}
