#pragma once
#include "Utilities.h"
#include "BoardMask.h"
#include "Move.h"
#include "Board.h"
#include "BoardNode.h"

class TreeNode {
public:
	static constexpr double epsilon = 1e-6;
	static const double explorationParameter;
	static uint numNodes() { return _numNodes; }
	
	TreeNode() funk;
	TreeNode(TreeNode* parent, Move move) funk;
	~TreeNode() funk;
	
	Board board() const funk { return _boardNode->board(_orientation, _moveCount); }
	
	Move move() const funk { return _move; }
	uint32 backwardVisits() const funk { return _boardNode->visits(); }
	sint32 backwardValue() const funk { return _orientation.colourFlipped()? -_boardNode->score() : _boardNode->score(); }
	double backwardScore(double logParentVisits) const funk;
	uint depth() const funk;
	uint numVisitedChildren() const funk;
	BoardMask visitedChildren() const funk;
	TreeNode* top() const funk;
	TreeNode* child(Move move) funk;
	TreeNode* firstChild() const funk { return _child; }
	TreeNode* sibling() const funk { return _sibling; }
	
	void vincent(TreeNode* child); ///< Favorite child, forget all other children
	
	void loadGames(const string& file) funk;
	void read(const string& filename, bool symmetrized = false) funk;
	void read(istream& in, Rotation rotation) funk;
	void write(const string& filename, uint treshold = 0) const funk;
	void write(ostream& out, uint treshold = 0) const funk;
	
	void writeOut(ostream& out, uint depth) const funk;
	
	void backwardRecurse(const Board& endGame, sint value) funk;
	void backwardUpdate(sint value) funk;
	
	void itterate(uint loops = 1);
	void selectAction(Board board) funk;
	bool isLeaf() const funk { return !_child; }
	void rollOut(const Board& board) funk;
	
	Move bestMove() const funk;
	
protected:
	friend ostream& operator<<(ostream& out, const TreeNode& treeNode) funk;
	static uint _numNodes;
	
	TreeNode* _parent;
	TreeNode* _child;
	TreeNode* _sibling;
	Move _move;
	Rotation _orientation;
	uint _moveCount;
	BoardNode* _boardNode;
	
	TreeNode* select(const Board& board) funk;
};
