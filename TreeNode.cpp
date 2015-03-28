#include "TreeNode.h"
#include "Random.h"
#include "DepthEstimator.h"
#include "HeatMap.h"
#include "TreeNode.conf.h"

uint TreeNode::_numNodes = 0;

TreeNode::TreeNode()
: _move(Move())
, _parent(nullptr)
, _child(nullptr)
, _sibling(nullptr)
, _orientation()
, _moveCount(0)
, _boardNode(nullptr)
{
	Board start;
	BoardNode::OrientedBoardNode obn = BoardNode::get(start);
	_orientation = obn.first;
	_boardNode = obn.second;
	_numNodes++;
}

TreeNode::TreeNode(TreeNode* parent, Move move)
: _move(move)
, _parent(parent)
, _child(nullptr)
, _sibling(nullptr)
, _boardNode(nullptr)
, _orientation()
, _moveCount(_parent->_moveCount + 1)
{
	Board board = _parent->board();
	board.playMove(move);
	BoardNode::OrientedBoardNode obn = BoardNode::get(board);
	_orientation = obn.first;
	_boardNode = obn.second;
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
	out << treeNode.visits() << " " << treeNode.score() << " ";
	out << treeNode.depth();
	for(const TreeNode* p = &treeNode; p; p = p->_parent)
		out << " " << p->_move;
	return out;
}

double TreeNode::uctScore(double logParentVisits) const
{
	const double v = visits() + epsilon;
	const double s = score();
	return (0.5 * + s) / v + explorationParameter * sqrt(logParentVisits / v);
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

Move TreeNode::bestMove() const
{
	#ifdef HEURISTIC
		return board().heuristicMove();
	#else
		assert(!isLeaf());
		
		// Find node with highest playout count
		TreeNode* best = nullptr;
		for(TreeNode* c = _child; c; c = c->_sibling)
			if(!best || c->visits() > best->visits())
				best = c;
			assert(best);
		return best->_move;
	#endif
}

void TreeNode::mcts()
{
	const bool fatPlayout = true;
	TreeNode* node = select();
	assert(node != nullptr);
	node = node->expand();
	int score = node->playOut(fatPlayout);
	node->updateScore(score);
}

TreeNode* TreeNode::select()
{
	return isLeaf() ? this : uctSelectChild()->select();
}

TreeNode* TreeNode::uctSelectChild()
{
	assert(!isLeaf());
	const float logParentVisits = log(visits() + 1);
	float bestValue = 0.0;
	TreeNode* selected = nullptr;
	for(TreeNode* c = _child; c; c = c->_sibling) {
		const float value = c->uctScore(logParentVisits);
		if(value > bestValue || (value == bestValue && entropy(1))) {
			bestValue = value;
			selected = c;
		}
	}
	return selected;
}

TreeNode* TreeNode::expand()
{
	assert(isLeaf());
	const Board b = board();
	for(Move move: b.validMoves()) {
		TreeNode* child = new TreeNode(this, move);
		child->_sibling = _child;
		_child = child;
	}
	return _child;
}

void TreeNode::selectAction()
{
	TreeNode* current = this;
	
	// Select an existing leaf
	while(!current->isLeaf()) {
		current = current->select();
		assert(current != nullptr);
	}
	
	// Expand one more
	{
		TreeNode* newNode = current->select();
		if(!newNode)
			return;
		current = newNode;
	}
	
	// Roll out randomly
	current->playOut();
	assert(!isLeaf());
}

int TreeNode::playOut(bool fat)
{
	Board fillOut = board();
	Board::Player player = fillOut.player();
	
	// Play till there is a winner
	for(;;) {
		Move move = fat ? fillOut.heuristicMove() : fillOut.randomMove();
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
	
	return winner == player ? 1 : -1;
}

void TreeNode::updateScore(sint score)
{
	_boardNode->addRecursive(1, _orientation.colourFlipped() ? -score : score);
	if(_parent)
		_parent->updateScore(score);
}
