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

void convertGames(const string& games, const string& out)
{
	Board board;
	TreeNode tree;
	tree.loadGames(games);
	tree.write(out);
}

void ponder(const string& filename)
{
	Board board;
	TreeNode tree;
	tree.read(filename, -1);
	tree.scaleStatistics(10);
	for(uint i = 0; i < 1000000; ++i) {
		if(i % 10000 == 0) {
			cerr << "nodes: " << TreeNode::numNodes()  << " (" << tree.backwardVisits() << " visits)" << " (";
			cerr << (TreeNode::numNodes() * sizeof(TreeNode) / (1024*1024))  << " MB)" << endl;
		}
		tree.selectAction(board);
	}
	tree.write(filename);
}

int main(int argc, char* argv[])
{
	cerr << "R " << argv[0]  << endl;
	cerr << "RAND_MAX = " << RAND_MAX << endl;
	cerr << "sizeof(float) = " << sizeof(float) << endl;
	cerr << "sizeof(uint) = " << sizeof(uint) << endl;
	cerr << "sizeof(uint128) = " << sizeof(uint128) << endl;
	cerr << "sizeof(void*) = " << sizeof(void*) << endl;
	cerr << "sizeof(BoardPoint) = " << sizeof(BoardPoint) << endl;
	cerr << "sizeof(Mov e) = " << sizeof(Move) << endl;
	cerr << "sizeof(BoardMask) = " << sizeof(BoardMask) << endl;
	cerr << "sizeof(Board) = " << sizeof(Board) << endl;
	cerr << "sizeof(TreeNode) = " << sizeof(TreeNode) << endl;
	srand(time(0));
	
	BoardPoint p;
	cin >> p;
	cout << p << endl;
	
	BoardMask b(p);
	for(BoardPoint p: b)
		cout << p << " ";
	cout << b << endl;
	cout << endl;
	
	b = p.neighbors();
	cout << b << endl;
	for(BoardPoint p: b)
		cout << p << " ";
	cout << endl;
	
	cout << b.randomPoint() << endl;
	cout << b.randomPoint() << endl;
	cout << b.randomPoint() << endl;
	cout << b.randomPoint() << endl;
	cout << b.randomPoint() << endl;
	cout << b.randomPoint() << endl;
	return 0;
	
	//convertGames("competitions-sym.txt", "games.bin");
	// convertGames("competitions-sym.txt", "itterated.bin");
	// ponder("itterated.bin");
	// return 0;
	
	GameInputOutput gio;
	//gio.tree()->read("/home/remco/Persoonlijk/Projects/Codecup/2014 Poly-Y/games.bin");
	//gio.tree()->read("/home/remco/Persoonlijk/Projects/Codecup/2014 Poly-Y/itterated.bin");
	gio.run();
	cerr << "Exit" << endl;
	return 0;
}
