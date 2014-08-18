#include "TreeNode.h"
#include "Random.h"
#include "DepthEstimator.h"
#include "HeatMap.h"

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
