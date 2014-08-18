#pragma once
#include "Utilities.h"
#include "BoardMask.h"
#include "Move.h"
#include "Board.h"

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
	bool isLeaf() const { return !_child; }
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
