#pragma once
#include "TreeNode.h"

class GameInputOutput {
public:
	GameInputOutput();
	~GameInputOutput();
	void run();
	void playMove(Move move);
	Move generateMove();
	
	TreeNode* tree() { return _tree; }
	
protected:
	Board _board aligned;
	TreeNode* _tree;
	TreeNode* _current;
};

