#pragma once
#include "Utilities.h"
#include "BoardMask.h"
#include "Move.h"
#include "Board.h"

class TreeNode {
public:
	static constexpr float epsilon = 1e-6;
	static const float explorationParameter;
	static uint numNodes() { return _numNodes; }
	
	TreeNode() funk;
	TreeNode(TreeNode* parent, Move move) funk;
	~TreeNode() funk;
	
	Board board() const funk { return _board; }
	
	Move move() const funk { return _move; }
	uint backwardVisits() const funk { return _backwardVisits; }
	uint backwardValue() const funk { return _backwardValue; }
	uint forwardVisits() const funk { return _forwardVisits; }
	uint forwardValue() const funk { return _forwardValue; }
	float backwardScore(float logParentVisits) const funk;
	float forwardScore(float logParentVisits) const funk;
	float alphaAmafScore(float logParentVisits, float alpha) const funk;
	float raveAlpha() const funk;
	float raveScore(float logParentVisits) const funk;
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
	
	void scaleStatistics(uint factor) funk;
	
	void backwardRecurse(const Board& endGame, float value) funk;
	void backwardUpdate(float value) funk;
	void forwardRecurse(const BoardMask& self, const BoardMask& other, float score) funk;
	void forwardUpdate(float score) funk;
	
	void itterate(uint action = 1);
	void selectAction(Board board) funk;
	bool isLeaf() const funk { return !_child; }
	void rollOut(const Board& board) funk;
	
	Move bestMove() const funk;
	
protected:
	friend ostream& operator<<(ostream& out, const TreeNode& treeNode) funk;
	static uint _numNodes;
	
	Board _board aligned;
	vector<Move> _moves;
	TreeNode* _parent;
	TreeNode* _child;
	TreeNode* _sibling;
	uint _backwardVisits;
	float _backwardValue;
	uint _forwardVisits;
	float _forwardValue;
	Move _move;
	TreeNode* select(const Board& board) funk;
};
