#include "TreeNode.h"

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
	assert(numTresholdChildren <= 255);
	
	// Write out this node
	if(_parent == nullptr) {
		out.put(0xff);
		out.put(0xff);
	} else {
		assert(_move.isValid());
		out.put(_move.from().position());
		out.put(_move.to().position());
	}
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

void TreeNode::read(const string& filename, bool symmetrized)
{
	cerr << "Reading " << filename << endl;
	
	// The initial board has four symmetries
	const vector<Rotation> normal = { Rotation::r0() };
	const vector<Rotation> symetries = { Rotation::r0(), Rotation::r2(), Rotation::mH(), Rotation::mV() };
	const vector<Rotation>& rotations = symmetrized ? symetries : normal;
	
	uint before = TreeNode::numNodes();
	ifstream file(filename, ifstream::in | ifstream::binary);
	for(Rotation rotation: rotations) {
		file.seekg(0);
		// Skip first node move
		assert(file.get() == 0xff);
		assert(file.get() == 0xff);
		read(file, rotation);
	}
	file.close();
	cerr << "Read " << (TreeNode::numNodes() - before) << " nodes" << endl;
}

void TreeNode::read(istream& in, Rotation rotation)
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
		uint from = in.get();
		uint to = in.get();
		Move move(from, to);
		
		// Apply rotation
		move = rotation(move);
		
		assert(move.isValid());
		TreeNode* c = child(move);
		c->read(in, rotation);
	}
}

void TreeNode::loadGames(const string& filename)
{
	ifstream file(filename);
	if(!file.good()) {
		cerr << "Could not read: " << filename << endl;
		return;
	}
	cerr << "Loading " << filename << "..." << flush;
	uint numGames = 0;
	for(string line; getline(file, line); ) {
		++numGames;
		
		// Iterate over all symmetries
		stringstream ss(line);
		Board board;
		TreeNode* gameState = this;
		while(ss.good()) {
			Move move;
			ss >> move;
			assert(move.isValid());
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
	cerr << " " << numGames << " games loaded." << endl;
}
