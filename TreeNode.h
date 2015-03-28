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
	
	// Tree
	TreeNode* top() const funk;
	TreeNode* firstChild() const funk { return _child; }
	TreeNode* sibling() const funk { return _sibling; }
	TreeNode* child(Move move) funk;
	bool isLeaf() const funk { return !_child; }
	uint depth() const funk;
	void vincent(TreeNode* child); ///< Favorite child, forget all other children
	
	Board board() const funk { return _boardNode->board(_orientation, _moveCount); }
	Move move() const funk { return _move; }
	Move bestMove() const funk;
	
	uint numVisitedChildren() const funk;
	BoardMask visitedChildren() const funk;
	
	uint32 visits() const funk { return _boardNode->visits(); }
	sint32 score() const funk { return _orientation.colourFlipped() ? -_boardNode->score() : _boardNode->score(); }
	double uctScore(double logParentVisits) const funk;
	
	void mcts() funk;
	TreeNode* select() funk;
	TreeNode* uctSelectChild() funk;
	TreeNode* expand() funk;
	void selectAction() funk;
	
	int playOut(bool fat = false) funk;
	
	void updateScore(sint score) funk;
	
	
	// I O
	void loadGames(const string& file) funk;
	void read(const string& filename, bool symmetrized = false) funk;
	void read(istream& in, Rotation rotation) funk;
	void write(const string& filename, uint treshold = 0) const funk;
	void write(ostream& out, uint treshold = 0) const funk;
	void writeOut(ostream& out, uint depth) const funk;
	
protected:
	friend ostream& operator<<(ostream& out, const TreeNode& treeNode) funk;
	static uint _numNodes;
	
	TreeNode* _parent;
	TreeNode* _child;
	TreeNode* _sibling;
	Move _move;
	uint _moveCount;
	Rotation _orientation;
	BoardNode* _boardNode;
};
