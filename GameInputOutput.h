#pragma once
#include "TreeNode.h"

class GameInputOutput {
public:
	GameInputOutput() funk;
	~GameInputOutput() funk;
	void run() funk;
	void playMove(Move move) funk;
	Move generateMove() funk;
	
	TreeNode* tree() { return _tree; }
	
protected:
	Board _board aligned;
	TreeNode* _tree;
	TreeNode* _current;
};

