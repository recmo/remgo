#include "Utilities.h"
#include "Random.h"
#include "Timer.h"
#include "BoardPoint.h"
#include "BoardMask.h"
#include "Move.h"

class HeatMap {
public:
	static HeatMap white;
	static HeatMap black;
	
	HeatMap();
	~HeatMap() { }
	
	void add(BoardMask moves);
	void scale(uint factor);
	Move bestMove(BoardMask moves) const;
	float score(Move move) const;
	
protected:
	friend ostream& operator<<(ostream& out, const HeatMap& heatmap);
	uint _max;
	uint _map[Move::maxIndex];
};

HeatMap HeatMap::white;
HeatMap HeatMap::black;

ostream& operator<<(ostream& out, const HeatMap& heatmap)
{
	for(uint i = 0; i < Move::maxIndex; ++i)
		out << " " << (float(heatmap._map[i]) / heatmap._max);
	return out;
}

HeatMap::HeatMap()
: _max(0)
, _map()
{
	for(uint i = 0; i < Move::maxIndex; ++i)
		_map[i] = 0;
}

void HeatMap::add(BoardMask moves)
{
	++_max;
	for(BoardMask::Iterator i = moves.itterator(); i; ++i)
		++_map[i->index()];
}

void HeatMap::scale(uint factor)
{
	_max /= factor;
	for(uint i = 0; i < Move::maxIndex; ++i)
		_map[i] /= factor;
}

Move HeatMap::bestMove(BoardMask moves) const
{
	uint bestScore = 0;
	Move bestMove;
	for(BoardMask::Iterator i = moves.itterator(); i; ++i) {
		const uint moveScore = _map[i->index()];
		if(moveScore <= bestScore)
			continue;
		bestScore = moveScore;
		bestMove = *i;
	}
	return bestMove;
}

float HeatMap::score(Move move) const
{
	if(_max == 0)
		return 0.5;
	return float(_map[move.index()]) / float(_max);
}


class Board {
public:
	Board(): _white(), _black(), _moveCount(0) { }
	~Board() { }
	bool operator==(const Board& other) const { return _moveCount == other._moveCount && _white == other._white && _black == other._black; }
	
	BoardMask nonSwapMoves() const { return ~(_white | _black); }
	bool gameOver() const;
	void playMove(Move move);
	
	uint moveCount() const { return _moveCount; }
	BoardMask white() const { return _white; }
	BoardMask black() const { return _black; }
	
	uint player() const { return (_moveCount & 1) ? 2 : 1; }
	uint winner() const;
	
	void bambooBridges();
	void randomFillUp();
	
protected:
	BoardMask _white;
	BoardMask _black;
	uint8 _moveCount;
};

std::ostream& operator<<(std::ostream& out, const Board& board)
{
	for(uint i = 1; i <= 106; ++i) {
		bool white = board.white().isSet(i);
		bool black = board.black().isSet(i);
		if(!white && !black)
			out << ".";
		if(white && !black)
			out << "W";
		if(!white && black)
			out << "B";
		if(white && black)
			out << "!";
	}
	return out;
}

void Board::playMove(Move move)
{
	// Increase move counter
	++_moveCount;
	
	// Swap move
	if(move == Move::Swap) {
		assert(_moveCount == 2);
		swap(_black, _white);
		return;
	}
	
	// Placement move
	assert(!_black.isSet(move));
	assert(!_white.isSet(move));
	if(_moveCount & 1) {
		_black.set(move);
	} else {
		_white.set(move);
	}
}

bool Board::gameOver() const
{
	if(!nonSwapMoves())
		return true;
	if(_white.controlledCorners() >= 3)
		return true;
	if(_black.controlledCorners() >= 3)
		return true;
	return false;
}

uint Board::winner() const
{
	if(_white.controlledCorners() >= 3)
		return 1;
	if(_black.controlledCorners() >= 3)
		return 2;
	return 0;
}

// Connects bamboo bridges randomly
void Board::bambooBridges()
{
	BoardMask free = ~(_black | _white);
	
	// Find groups and extensions
	vector<BoardMask> whiteGroups = _white.groups();
	vector<BoardMask> whiteExtensions;
	for(const BoardMask& g: whiteGroups)
		whiteExtensions.push_back(g.expanded());
	vector<BoardMask> blackGroups = _black.groups();
	vector<BoardMask> blackExtensions;
	for(const BoardMask& g: blackGroups)
		blackExtensions.push_back(g.expanded());
	
	// Randomly play black or white
	uint whiteIndex = 0;
	uint blackIndex = 0;
	while((whiteIndex < whiteGroups.size()) || (blackIndex < blackGroups.size())) {
		uint i;
		BoardMask group;
		BoardMask* player = nullptr;
		BoardMask* opponent = nullptr;
		vector<BoardMask>* self = nullptr;
		if(whiteIndex != whiteGroups.size() && (blackIndex == blackGroups.size() || entropy(2))) {
			i = whiteIndex++;
			group = whiteGroups[i];
			player = &_white;
			opponent = &_black;
			self = &whiteExtensions;
		} else {
			i = blackIndex++;
			group = blackGroups[i];
			player = &_black;
			opponent = &_white;
			self = &blackExtensions;
		}
		BoardMask extension = self->at(i);
		
		// Borders
		for(uint j = 0; j < 5; j++) {
			BoardMask border = BoardMask::borders[j];
			if(group & border)
				continue; // Already connected to border
			BoardMask bamboo = extension & border & free;
			if(bamboo.popcount() == 2) {
				BoardPoint p = bamboo.randomPoint();
				player->set(p);
				*opponent |= bamboo - *player;
				free -= bamboo;
			}
		}
		
		// Other groups
		for(uint j = 0; j < i; j++) {
			BoardMask bamboo = extension & self->at(j) & free;
			if(bamboo.popcount() == 2) {
				BoardPoint p = bamboo.randomPoint();
				player->set(p);
				*opponent |= bamboo - *player;
				free -= bamboo;
			}
		}
	}
}

void Board::randomFillUp()
{
	BoardMask free = ~(_black | _white);
	uint whiteStones = 53 - _black.popcount();
	uint blackStones = 53 - _white.popcount();
	
	// Fill up with equal amounts of stones
	for(auto i = free.itterator(); i; i++) {
		if(blackOrWhite(blackStones, whiteStones)) {
			_white.set(*i);
			whiteStones--;
		} else {
			_black.set(*i);
			blackStones--;
		}
	}
	assert(whiteStones == 0 && blackStones == 0);
}

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

class TreeNode {
public:
	static constexpr int nActions = 5;
	static constexpr float epsilon = 1e-6;
	static constexpr float explorationParameter = sqrt(2.0);
	static uint numNodes() { return _numNodes; }
	
	TreeNode();
	TreeNode(TreeNode* parent, Move move);
	~TreeNode();
	
	Move move() const { return _move; }
	uint backwardVisits() const { return _backwardVisits; }
	uint backwardValue() const { return _backwardValue; }
	uint forwardVisits() const { return _forwardVisits; }
	uint forwardValue() const { return _forwardValue; }
	float backwardScore(float logParentVisits) const;
	float forwardScore(float logParentVisits) const;
	float alphaAmafScore(float logParentVisits, float alpha) const;
	float raveAlpha() const;
	float raveScore(float logParentVisits) const;
	uint depth() const;
	uint numVisitedChildren() const;
	BoardMask visitedChildren() const;
	TreeNode* top() const;
	TreeNode* child(Move move);
	
	void vincent(TreeNode* child); ///< Favorite child, forget all other children
	
	void loadGames(const string& file);
	void read(const string& filename, uint rotation = 0);
	void read(istream& in, uint rotation = 0);
	void write(const string& filename, uint treshold = 0) const;
	void write(ostream& out, uint treshold = 0) const;
	
	void writeOut(ostream& out, uint depth) const;
	
	void scaleStatistics(uint factor);
	
	void backwardRecurse(const Board& endGame, float value);
	void backwardUpdate(float value);
	void forwardRecurse(const BoardMask& self, const BoardMask& other, float score);
	void forwardUpdate(float score);
	
	void selectAction(Board board);
	bool isLeaf() { return !_child; }
	   void rollOut(const Board& board);
	
	Move bestMove() const;
	
protected:
	friend ostream& operator<<(ostream& out, const TreeNode& treeNode);
	static uint _numNodes;
	
	Move _move;
	uint _backwardVisits;
	float _backwardValue;
	uint _forwardVisits;
	float _forwardValue;
	TreeNode* _parent;
	TreeNode* _child;
	TreeNode* _sibling;
	TreeNode* select(const Board& board);
};

uint TreeNode::_numNodes = 0;

TreeNode::TreeNode()
: _move(Move())
, _backwardVisits(0)
, _backwardValue(0.0)
, _forwardVisits(0)
, _forwardValue(0.0)
, _parent(nullptr)
, _child(nullptr)
, _sibling(nullptr)
{
	_numNodes++;
}

TreeNode::TreeNode(TreeNode* parent, Move move)
: _move(move)
, _backwardVisits(0)
, _backwardValue(0.0)
, _forwardVisits(0)
, _forwardValue(0.0)
, _parent(parent)
, _child(nullptr)
, _sibling(nullptr)
{
	_numNodes++;
}

TreeNode::~TreeNode()
{
	for(TreeNode* c = _child, *next = nullptr; c; c = next) {
		next = c->_sibling;
		delete c;
	}
	_numNodes--;
}

std::ostream& operator<<(std::ostream& out, const TreeNode& treeNode)
{
	out << treeNode.backwardVisits() << " " << treeNode.backwardValue() << " ";
	out << treeNode.forwardVisits() << " " << treeNode.forwardValue() << " ";
	out << treeNode.depth();
	for(const TreeNode* p = &treeNode; p; p = p->_parent)
		out << " " << p->_move;
	return out;
}

float TreeNode::backwardScore(float logParentVisits) const
{
	return backwardValue() / (backwardVisits() + epsilon) + explorationParameter * sqrt(logParentVisits / (backwardVisits() + epsilon));
}

float TreeNode::forwardScore(float logParentVisits) const
{
	return forwardValue() / (forwardVisits() + epsilon) + explorationParameter * sqrt(logParentVisits / (forwardVisits() + epsilon));
}

float TreeNode::alphaAmafScore(float logParentVisits, float alpha) const
{
	return ((1.0 - alpha) * backwardScore(logParentVisits)) + (alpha * forwardScore(logParentVisits));
}

float TreeNode::raveAlpha() const
{
	const float parameterValue = 50000;
	if(backwardVisits() > parameterValue)
		return 0.0;
	return (backwardVisits() - parameterValue) / parameterValue;
}

float TreeNode::raveScore(float logParentVisits) const
{
	return alphaAmafScore(logParentVisits, raveAlpha());
}

TreeNode* TreeNode::child(Move move)
{
	assert(move.isValid());
	
	// See if there already is an childnode for it
	for(TreeNode* c = _child; c; c = c->_sibling)
		if(c->_move == move)
			return c;
	
	// Construct a new child node
	TreeNode* node = new TreeNode(this, move);
	node->_sibling = _child;
	_child = node;
	return node;
}

TreeNode* TreeNode::top() const
{
	for(TreeNode* p = _parent; p; p = p->_parent)
		if(!p->_parent)
			return p;
	return const_cast<TreeNode*>(this);
}

uint TreeNode::depth() const
{
	uint result = 0;
	for(TreeNode* p = _parent; p; p = p->_parent)
		++result;
	return result;
}

uint TreeNode::numVisitedChildren() const
{
	uint result = 0;
	for(TreeNode* c = _child; c; c = c->_sibling)
		++result;
	return result;
}

BoardMask TreeNode::visitedChildren() const
{
	BoardMask result;
	for(TreeNode* c = _child; c; c = c->_sibling)
		result.set(c->_move);
	return result;
}

void TreeNode::vincent(TreeNode* child)
{
	// Forget other children
	for(TreeNode* c = _child, *next = nullptr; c; c = next) {
		next = c->_sibling;
		if(c != child) {
			delete c;
		}
	}
	_child = child;
	_child->_sibling = nullptr;
}

void TreeNode::backwardRecurse(const Board& endGame, float value)
{
	backwardUpdate(value);
	if(_parent)
		_parent->backwardRecurse(endGame, 1.0 - value);
	else
		/// @todo Colours are incorrect if there was a swap!!
		forwardRecurse(endGame.white(), endGame.black(), value);
}

void TreeNode::backwardUpdate(float value)
{
	++_backwardValue;
	_backwardValue += value;
}

void TreeNode::forwardRecurse(const BoardMask& self, const BoardMask& other, float score)
{
	// Swap node
	if(_move == Move::Swap) {
		forwardUpdate(score);
		score = 1.0 - score;
		for(TreeNode* c = _child; c; c = c->_sibling)
			c->forwardRecurse(self, other, score);
		
	// Regular moves
	} else if(_move.isValid()) {
		if(!self.isSet(_move))
			return;
		forwardUpdate(score);
		score = 1.0 - score;
		for(TreeNode* c = _child; c; c = c->_sibling)
			c->forwardRecurse(other, self, score);
		
	// Root node
	} else {
		for(TreeNode* c = _child; c; c = c->_sibling)
			c->forwardRecurse(self, other, score);
	}
}

void TreeNode::forwardUpdate(float score)
{
	++_forwardVisits;
	_forwardValue += score;
}

void TreeNode::writeOut(ostream& out, uint treshold) const
{
	if(backwardVisits() < treshold)
		return;
	out << *this << endl;
	for(TreeNode* c = _child; c; c = c->_sibling)
		c->writeOut(out, treshold);
}

void TreeNode::write(const string& filename, uint treshold) const
{
	ofstream file(filename, ofstream::out | ofstream::trunc | ofstream::binary);
	write(file, treshold);
	file.close();
}

void TreeNode::write(ostream& out, uint treshold) const
{
	assert(out.good());
	
	// Skip if the number of visits is insufficient
	if(backwardVisits() < treshold)
		return;
	
	// Count number of children that pass the threshold
	uint numTresholdChildren = 0;
	for(TreeNode* c = _child; c; c = c->_sibling)
		if(c->backwardVisits() >= treshold)
			++numTresholdChildren;
	
	// Write out this node
	out.put(_move.index());
	out.write(reinterpret_cast<const char*>(&_backwardVisits), sizeof(_backwardVisits));
	out.write(reinterpret_cast<const char*>(&_backwardValue), sizeof(_backwardValue));
	out.write(reinterpret_cast<const char*>(&_forwardVisits), sizeof(_forwardVisits));
	out.write(reinterpret_cast<const char*>(&_forwardValue), sizeof(_forwardValue));
	out.put(numTresholdChildren);
	
	// Write out child nodes
	for(TreeNode* c = _child; c; c = c->_sibling)
		if(c->backwardVisits() >= treshold)
			c->write(out, treshold);
}

void TreeNode::read(const string& filename, uint rotation)
{
	cerr << "Reading " << filename << endl;
	if(rotation == -1) {
		for(rotation = 0; rotation < Move::maxRotation; ++rotation)
			read(filename, rotation);
		return;
	}
	uint before = TreeNode::numNodes();
	ifstream file(filename, ifstream::in | ifstream::binary);
	assert(file.get() == 0xff); // Skip first node move
	read(file, rotation);
	file.close();
	cerr << "Read " << (TreeNode::numNodes() - before) << " nodes" << endl;
}

void TreeNode::read(istream& in, uint rotation)
{
	assert(in.good());
	
	// Read this node
	uint visits;
	float value;
	in.read(reinterpret_cast<char*>(&visits), sizeof(visits));
	in.read(reinterpret_cast<char*>(&value), sizeof(value));
	_forwardVisits += visits;
	_forwardValue += value;
	in.read(reinterpret_cast<char*>(&visits), sizeof(visits));
	in.read(reinterpret_cast<char*>(&value), sizeof(value));
	_backwardVisits += visits;
	_backwardValue += value;
	
	// Read child nodes
	uint numChildren = in.get();
	for(uint i = 0; i < numChildren; ++i) {
		Move move = Move::fromIndex(in.get());
		assert(move.isValid());
		if(move != Move::Swap)
			move.rotate(rotation);
		TreeNode* c = child(move);
		c->read(in);
	}
}

TreeNode* TreeNode::select(const Board& board)
{
	const HeatMap& heatmap = (board.player() == 1) ? HeatMap::white : HeatMap::black;
	
	// Index over moves
	bool valid[Move::numIndices];
	float values[Move::numIndices];
	
	// Initialize moves with heatmap
	/// @todo Unexplored should outmax explored
	for(uint i = 0; i < Move::numIndices; ++i) {
		values[i] = heatmap.score(Move::fromIndex(i));
		valid[i] = false;
	}
	
	// Enable swap move if allowed
	if(board.moveCount() == 1)
		valid[Move::Swap.index()] = true;
	
	// Enable non swap moves
	BoardMask moves = board.nonSwapMoves();
	for(BoardMask::Iterator i = moves.itterator(); i; ++i)
		valid[i->index()] = true;
	
	// Load existing child data
	const float logParentVisits = log(this->backwardVisits() + 1);
	for(TreeNode* c = _child; c; c = c->_sibling) {
		values[c->_move.index()] = c->raveScore(logParentVisits);
	}
	
	// UCT select node
	uint selectedIndex = 0;
	float bestValue = 0.0;
	for(uint i = 0; i < Move::numIndices; ++i) {
		if(!valid[i])
			continue;
		if(values[i] > bestValue || (values[i] == bestValue && entropy(1))) {
			selectedIndex = i;
			bestValue = values[i];
		}
	}
	return child(Move::fromIndex(selectedIndex));
}

void TreeNode::loadGames(const string& filename)
{
	ifstream file(filename);
	if(!file.good()) {
		cerr << "Could not read: " << filename << endl;
		return;
	}
	for(string line; getline(file, line); ) {
		
		// Iterate over all symmetries
		for(uint rotation = 0; rotation < BoardPoint::maxRotation; ++rotation) {
			stringstream ss(line);
			Board board;
			TreeNode* gameState = this;
			while(ss.good()) {
				Move move;
				ss >> move;
				assert(move.isValid());
				if(move != Move::Swap)
					move.rotate(rotation);
				board.playMove(move);
				gameState = gameState->child(move);
				assert(gameState);
			}
			if(!board.gameOver()) {
				cerr << "!!! Not entire game!" << endl;
				continue;
			}
			
			/// @todo Commit score
			float value = (board.winner() == board.player()) ? 1.0 : 0.0;
			// value = 1.0 - value;
			for(uint i = 0; i < 10; ++i)
				gameState->backwardRecurse(board, value);
		}
	}
}

void TreeNode::selectAction(Board board)
{
	TreeNode* current = this;
	while(!current->isLeaf()) {
		current = current->select(board);
		board.playMove(current->_move);
	}
	if(!current->isLeaf()) {
		TreeNode* newNode = current->select(board);
		assert(newNode);
		current = newNode;
		board.playMove(current->_move);
	}
	current->rollOut(board);
}

Move TreeNode::bestMove() const
{
	// Find node with highest playout count
	TreeNode* best = nullptr;
	for(TreeNode* c = _child; c; c = c->_sibling)
		if(!best || c->backwardVisits() > best->backwardVisits())
			best = c;
	assert(best);
	return best->_move;
}

void TreeNode::rollOut(const Board& board)
{
	Board fillOut(board);
	fillOut.bambooBridges();
	fillOut.randomFillUp();
	
	// Find the winner
	uint winner = 0;
	BoardMask winningSet = fillOut.white().winningSet();
	if(winningSet) {
		winner = 1; // White won
	} else {
		winningSet = fillOut.black().winningSet();
		if(winningSet)
			winner = 2; // black won
	}
	
	// No winner
	
	// Update HeatMap
	if(winner == 1)
		HeatMap::white.add(fillOut.white());
	if(winner == 2)
		HeatMap::black.add(fillOut.black());
	
	// Estimate depth
	if(winner == 1) {
		BoardMask addedPieces = winningSet - board.white();
		uint depth = board.moveCount() + (2 * addedPieces.popcount());
		DepthEstimator::instance.addEstimate(depth);
	}
	if(winner == 2) {
		BoardMask addedPieces = winningSet - board.black();
		uint depth = board.moveCount() + (2 * addedPieces.popcount());
		DepthEstimator::instance.addEstimate(depth);
	}
	
	// Update scores
	if(winner == 0)
		backwardRecurse(fillOut, 0.5);
	else
		backwardRecurse(fillOut, (winner == board.player()) ? 1.0 : 0.0);
}

void TreeNode::scaleStatistics(uint factor)
{

	_backwardVisits /= factor;
	_backwardValue /= factor;
	_forwardVisits /= factor;
	_forwardValue /= factor;
	for(TreeNode* c = _child; c; c = c->_sibling)
		c->scaleStatistics(factor);
}

class GameInputOutput {
public:
	GameInputOutput();
	~GameInputOutput();
	void run();
	void playMove(Move move);
	Move generateMove();
	
	TreeNode* tree() { return _tree; }
	
protected:
	Board _board;
	TreeNode* _tree;
	TreeNode* _current;
};

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
	cerr << "sizeof(void*) = " << sizeof(void*) << endl;
	cerr << "sizeof(BoardPoint) = " << sizeof(BoardPoint) << endl;
	cerr << "sizeof(Mov e) = " << sizeof(Move) << endl;
	cerr << "sizeof(BoardMask) = " << sizeof(BoardMask) << endl;
	cerr << "sizeof(Board) = " << sizeof(Board) << endl;
	cerr << "sizeof(TreeNode) = " << sizeof(TreeNode) << endl;
	srand(time(0));
	
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
