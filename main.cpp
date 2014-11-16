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
#include "Dijkstra.h"

/// TODO: Can we combine MCTS with the cellular automata supperoptimization?
/// Like hashlife—I think we can

int main(int argc, char* argv[]) funk;

uint64 hashBoard(const Board& board);

void benchmarkRollout()
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

void benchmarkSelect()
{
	// Benchmark
	const Board startingBoard;
	TreeNode root;
	uint simulations = 0;
	Timer::instance.update();
	float start = Timer::instance.remaining();
	while(Timer::instance.remaining() > 20.0) {
		root.selectAction(startingBoard);
		++simulations;
		Timer::instance.update();
	}
	float stop = Timer::instance.remaining();
	cerr << "Time: " << (start - stop) << endl;
	cerr << "Simulations: " << simulations << endl;
	cerr << "S/sec: " << (float(simulations)/(start - stop)) << endl;
}

inline uint64 rotate_left(uint64 a, int p) { return (a << (p % 64)) | (a >> (64 - (p % 64))); }

const uint64 wall = 0xf07e84658f112f0fUL;
const uint64 empty = 0xd60d74b3eb29093dUL;
const uint64 player = 0x9faa2a2dd3178d69UL;
const uint64 opponent = 0xab76d777a0229067UL;

bool oriented = true;

uint64 hashRecursive(uint64 tl, uint64 tr, uint64 bl, uint64 br)
{
	if(oriented) {
		// Orient the cell
		// Enforce tl is min and tr < bl
		uint64 min = tl;
		if(tr < min)
			min = tr;
		if(bl < min)
			min = bl;
		if(br < min)
			min = br;
		while(tr != min) {
			// Quarter rotation
			uint64 temp = tl;
			tl = tr;
			tr = br;
			br = bl;
			bl = temp;
		}
		if(tr > bl) {
			// Mirror in tl-br diagonal
			uint64 temp = tr;
			tr = bl;
			bl = temp;
		}
	}
	
	// Multiply each cell by a unique 64 bit prime
	tl *= 0xe4e5a4def3ae754fUL;
	tr *= 0xf0d56d32e10c8243UL;
	bl *= 0xfabfd7310ee77e0bUL;
	br *= 0xcafaf37f45c82715UL;
	
	// Xor them together
	tl ^= tr;
	bl ^= br;
	tl ^= bl;
	
	// Mix them some more
	tl = rotate_left(tl, 31);
	tl *= 0x4cf5ad432745937fUL;
	tl ^= tl >> 33;
	return tl;
}

uint64 hashRegion(const Board& board, int x, int y, uint s)
{
	assert(s > 0);
	
	// Single cell case
	if(s == 1) {
		if(x < 0 || y < 0 || x > 10 || y > 10)
			return wall;
		if(board.playerPieces().isSet(BoardPoint(x, y)))
			return player;
		if(board.opponentPieces().isSet(BoardPoint(x, y)))
			return opponent;
		return empty;
	}
	
	// Go recursive
	s /= 2;
	
	return hashRecursive(
		hashRegion(board, x, y, s),
		hashRegion(board, x + s, y, s),
		hashRegion(board, x, y + s, s),
		hashRegion(board, x + s, y + s, s)
	);
}

uint64 hashBoard(const Board& board)
{
	return hashRegion(board, -2, -2, 16);
}

// By sorting on the eightfold symmetry we can shrink the table by a factor 8 at each level

// By doing multiple (9) splits we can cover all boundaries
#include <set>
#include <unordered_map>

void split(const Board& board, int x = -2, int y = -2, uint s = 16)
{
	uint64 hash = hashRegion(board, x, y, s);
	
	s /= 2;
	
	if(s == 0) {
		// Single cell case
	}
	
	if(s >= 1) {
		// Four corners
		split(board, x    , y    , s);
		split(board, x + s, y    , s);
		split(board, x    , y + s, s);
		split(board, x + s, y + s, s);
	}
	
	if(s >= 2) {
		uint z = s / 2;
		
		// Overlaps
		split(board, x    , y + z, s);
		split(board, x + s, y + z, s);
		split(board, x + z, y    , s);
		split(board, x + z, y + s, s);
		
		// Center
		split(board, x + z, y + z, s);
	}
}

void split(TreeNode* node)
{
	cerr << "." << flush;
	split(node->board());
	for(TreeNode* c = node->firstChild(); c; c = c->sibling())
		split(c);
}

class Piece {
public:
	
	uint64 hash() const;
	
private:
	Piece() { }
	~Piece() { }
	static std::unordered_map<uint64, Piece*> _pieces;
	uint64 _hash;
	uint _height;
	uint _orientations[4];
	Piece* _corners[4];
};

// std::hash implementation
namespace std {
	template<> struct hash<Piece> {
		typedef Piece argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const {
			return s.hash();
		}
	};
}

int main(int argc, char* argv[])
{
	Timer::instance.start();
	cerr << "R " << argv[0]  << endl;
	cerr << "RAND_MAX = " << RAND_MAX << endl;
	cerr << "sizeof(uint) = " << sizeof(uint) << endl;
	cerr << "sizeof(uint64) = " << sizeof(uint64) << endl;
	cerr << "sizeof(uint128) = " << sizeof(uint128) << endl;
	cerr << "sizeof(void*) = " << sizeof(void*) << endl;
	cerr << "sizeof(std::size_t) = " << sizeof(std::size_t) << endl;
	cerr << "sizeof(Rotation) = " << sizeof(Rotation) << endl;
	cerr << "sizeof(BoardPoint) = " << sizeof(BoardPoint) << endl;
	cerr << "sizeof(Move) = " << sizeof(Move) << endl;
	cerr << "sizeof(BoardMask) = " << sizeof(BoardMask) << endl;
	cerr << "sizeof(Board) = " << sizeof(Board) << endl;
	cerr << "sizeof(TreeNode) = " << sizeof(TreeNode) << endl;
	srand(time(0));
	BoardMask::initialize();
	
	TreeNode gameTree;
	gameTree.read("games.bin", true);
	gameTree.write("games.symmetric2.bin");
		
	return 0;
	
	GameInputOutput gio;
	gio.run();
	cerr << "Exit" << endl;
	Timer::instance.stop();
	return 0;
}
