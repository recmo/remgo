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
	
	TreeNode();
	TreeNode(TreeNode* parent, Move move);
	~TreeNode();
	
	// Tree
	TreeNode* top() const;
	TreeNode* firstChild() const { return _child; }
	TreeNode* sibling() const { return _sibling; }
	TreeNode* child(Move move);
	bool isLeaf() const { return !_child; }
	uint depth() const;
	void vincent(TreeNode* child); ///< Favorite child, forget all other children
	
	Board board() const { return _boardNode->board(_orientation, _moveCount); }
	Move move() const { return _move; }
	Move bestMove() const;
	
	uint numVisitedChildren() const;
	BoardMask visitedChildren() const;
	
	uint32 visits() const { return _boardNode->visits(); }
	sint32 score() const { return _orientation.colourFlipped() ? -_boardNode->score() : _boardNode->score(); }
	double uctScore(double logParentVisits) const;
	
	void mcts();
	TreeNode* select();
	TreeNode* uctSelectChild();
	TreeNode* expand();
	void selectAction();
	
	int playOut(bool fat = false);
	
	void updateScore(sint score);
	
	
	// I O
	void loadGames(const string& file);
	void read(const string& filename, bool symmetrized = false);
	void read(istream& in, Rotation rotation);
	void write(const string& filename, uint treshold = 0) const;
	void write(ostream& out, uint treshold = 0) const;
	void writeOut(ostream& out, uint depth) const;
	
protected:
	friend ostream& operator<<(ostream& out, const TreeNode& treeNode);
	static uint _numNodes;
	
	TreeNode* _parent;
	TreeNode* _child;
	TreeNode* _sibling;
	Move _move;
	uint _moveCount;
	Rotation _orientation;
	BoardNode* _boardNode;
};
