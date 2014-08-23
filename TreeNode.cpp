#include "TreeNode.h"
#include "Random.h"
#include "DepthEstimator.h"
#include "HeatMap.h"

uint TreeNode::_numNodes = 0;

TreeNode::TreeNode()
: _move(Move())
, _board()
, _moves()
, _backwardVisits(0)
, _backwardValue(0.0)
, _forwardVisits(0)
, _forwardValue(0.0)
, _parent(nullptr)
, _child(nullptr)
, _sibling(nullptr)
{
	_moves = _board.validMoves();
	_numNodes++;
}

TreeNode::TreeNode(TreeNode* parent, Move move)
: _move(move)
, _board(parent->_board)
, _moves()
, _backwardVisits(0)
, _backwardValue(0.0)
, _forwardVisits(0)
, _forwardValue(0.0)
, _parent(parent)
, _child(nullptr)
, _sibling(nullptr)
{
	_board.playMove(move);
	_moves = _board.validMoves();
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
	for(TreeNode* c = _child; c; c = c->_sibling) {
		if(c->_move == move) {
			return c;
		}
	}
	
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
		result.set(c->_move.from());
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

void TreeNode::backwardRecurse(const Board& endGame, float score)
{
	backwardUpdate(score);
	if(_parent)
		_parent->backwardRecurse(endGame, 1.0 - score);
	else
		forwardRecurse(endGame.white(), endGame.black(), score);
}

void TreeNode::backwardUpdate(float score)
{
	++_backwardVisits;
	_backwardValue += score;
}

void TreeNode::forwardRecurse(const BoardMask& self, const BoardMask& other, float score)
{
	// Regular moves
	if(_move.isValid()) {
		if(!self.isSet(_move.to()))
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

TreeNode* TreeNode::select(const Board& board)
{
	// Explore unexplored nodes first first
	if(!_moves.empty()) {
		// Find the most suitable move
		Move m = _moves.front();
		_moves.erase(_moves.begin());
		
		// Create a new child
		return child(m);
	}
	
	// No moves to explore, check for children
	if(!_child)
		return nullptr;
	
	// UCT select node
	const float logParentVisits = log(this->backwardVisits() + 1);
	float bestValue = 0.0;
	TreeNode* selected = nullptr;
	for(TreeNode* c = _child; c; c = c->_sibling) {
		Move childMove = c->_move;
		const float value = c->backwardScore(logParentVisits);
		if(value > bestValue || (value == bestValue && entropy(1))) {
			bestValue = value;
			selected = c;
		}
	}
	return selected;
}

void TreeNode::selectAction(Board board)
{
	TreeNode* current = this;
	
	// Select an existing leaf
	while(!current->isLeaf()) {
		current = current->select(board);
		board.playMove(current->_move);
	}
	
	// Expand one more
	{
		TreeNode* newNode = current->select(board);
		if(!newNode)
			return;
		current = newNode;
		board.playMove(current->_move);
	}
	
	// Roll out randomly
	current->rollOut(board);
	assert(!isLeaf());
}

Move TreeNode::bestMove() const
{
	#ifdef HEURISTIC
		return _board.heuristicMove();
	#else
		assert(!isLeaf());
		
		// Find node with highest playout count
		TreeNode* best = nullptr;
		for(TreeNode* c = _child; c; c = c->_sibling)
			if(!best || c->backwardVisits() > best->backwardVisits())
				best = c;
		assert(best);
		return best->_move;
	#endif
}

void TreeNode::rollOut(const Board& board)
{
	Board fillOut(board);
	
	// Play till there is a winner
	for(;;) {
		Move move = fillOut.randomMove();
		if(!move.isValid())
			break;
		fillOut.playMove(move);
	}
	
	// Update the depth estimator
	DepthEstimator::instance.addEstimate(fillOut.moveCount());
	
	// Find the winner
	assert(fillOut.gameOver());
	Board::Player winner = fillOut.winner();
	assert(winner == Board::Black || winner == Board::White);
	
	// Update scores
	backwardRecurse(fillOut, (winner == board.player()) ? 0.0 : 1.0);
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
