#include "BoardNode.h"

std::unordered_map<uint64, BoardNode*> BoardNode::_fragments;
BoardNode BoardNode::_wall(0xf07e84658f112f0fUL);
BoardNode BoardNode::_empty(0xd60d74b3eb29093dUL);
BoardNode BoardNode::_player(0x9faa2a2dd3178d69UL);
BoardNode BoardNode::_opponent(0xab76d777a0229067UL);

std::ostream& operator<<(std::ostream& out, const BoardNode& boardNode)
{
	out.width(16);
	out.fill('0');
	out << right << hex << boardNode.hash() << dec << " " << boardNode.height() << endl;
	uint size = boardNode.size();
	char buffer[size][size];
	for(uint i = 0; i < size; ++i)
	for(uint j = 0; j < size; ++j)
		buffer[i][j] = '?';
	boardNode.print(&buffer[0][0]);
	for(uint r = 0; r < size; ++r) {
		for(uint c = 0; c < size; ++c)
			out << buffer[r][c];
		out << endl;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const BoardNode::OrientedBoardNode& boardNode)
{
	out.width(16);
	out.fill('0');
	out << right << hex << boardNode.second->hash() << dec << " " << boardNode.second->height() << endl;
	uint size = boardNode.second->size();
	char buffer[size][size];
	for(uint i = 0; i < size; ++i)
	for(uint j = 0; j < size; ++j)
		buffer[i][j] = '?';
	boardNode.second->print(&buffer[0][0], size, boardNode.first);
	for(uint r = 0; r < size; ++r) {
		for(uint c = 0; c < size; ++c)
			out << buffer[r][c];
		out << endl;
	}
	return out;
}

void BoardNode::initialize()
{
	_fragments[_wall.hash()] = &_wall;
	_fragments[_empty.hash()] = &_empty;
	_fragments[_player.hash()] = &_player;
	_fragments[_opponent.hash()] = &_opponent;
}

void BoardNode::dumpFragments(ostream& out)
{
	for(auto i: _fragments)
		out << *(i.second) << endl;
}


BoardNode::OrientedBoardNode BoardNode::get(const Board& board)
{
	return get(board, -2, -2, 16);
}

BoardNode::OrientedBoardNode BoardNode::get(const Board& board, int x, int y, uint s)
{
	assert(s > 0);
	
	// Single cell case
	if(s == 1) {
		if(x < 0 || y < 0 || x > 10 || y > 10)
			return make_pair(Rotation(), &_wall);
		if(board.playerPieces().isSet(BoardPoint(x, y)))
			return make_pair(Rotation(), &_player);
		if(board.opponentPieces().isSet(BoardPoint(x, y)))
			return make_pair(Rotation(), &_opponent);
		return make_pair(Rotation(), &_empty);
	}
	
	// Go recursive
	s /= 2;
	return get(
		get(board, x, y, s),
		get(board, x + s, y, s),
		get(board, x, y + s, s),
		get(board, x + s, y + s, s)
	);
}

BoardNode::OrientedBoardNode BoardNode::get(const BoardNode::OrientedBoardNode corners[4])
{
	// Find the normalized orientation for the requested piece
	// Try all orientations and use the one with the lowest hash
	/// TODO: Better way?
	
	// TODO: this would also be a good place to divide out board symmetries, instead of in the 'normalize' function
	
	Rotation orientation;
	uint64 lowestHash = std::numeric_limits<uint64>::max();
	BoardNode lowestNode;
	for(Rotation r: Rotation::all) {
		BoardNode current(corners, r);
		if(current.hash() < lowestHash) {
			orientation = r.inverted();
			lowestHash = current.hash();
			lowestNode = current;
		}
	}
	
	/*
	/// TODO
	uint s = 0;
	for(Rotation r: Rotation::all) {
		BoardNode current(corners, r);
		if(current.hash() == lowestHash)
			++s;
	}
	if(s > 1) {
		for(Rotation r: Rotation::all) {
			BoardNode current(corners, r);
			if(current.hash() == lowestHash)
				cerr << orientation * r << " ";
		}
		cerr << endl;
	}
	*/
	
	// Now deduplicate that node
	BoardNode* node = _fragments[lowestHash];
	if(node == nullptr) {
		// Does not exist yet, create a new one
		node = new BoardNode(lowestNode);
		_fragments[node->hash()] = node;
	}
	assert(node->hash() == lowestHash);
	
	return make_pair(node->normalize(orientation), node);
}

BoardNode::OrientedBoardNode BoardNode::get(const BoardNode::OrientedBoardNode& tl, const BoardNode::OrientedBoardNode& tr, const BoardNode::OrientedBoardNode& bl, const BoardNode::OrientedBoardNode& br)
{
	BoardNode::OrientedBoardNode corners[4] = {tl, tr, bl, br};
	return get(corners);
}

BoardNode::BoardNode()
: _hash(0)
, _height(0)
, _symmetries()
, _orientations{Rotation(), Rotation(), Rotation(), Rotation()}
, _corners{nullptr, nullptr, nullptr, nullptr}
{
}

BoardNode::BoardNode(uint64 hash)
: _hash(0)
, _height(0)
, _symmetries(SymmetryGroup::all())
, _orientations{Rotation(), Rotation(), Rotation(), Rotation()}
, _corners{nullptr, nullptr, nullptr, nullptr}
{
	_hash = hash;
}

BoardNode::BoardNode(const BoardNode::OrientedBoardNode corners[4], Rotation rotation)
: _hash(0)
, _height(0)
, _symmetries()
, _orientations{Rotation(), Rotation(), Rotation(), Rotation()}
, _corners{nullptr, nullptr, nullptr, nullptr}
{
	for(uint i = 0; i < 4; ++i) {
		assert(corners[i].second != nullptr);
		_corners[i] = corners[i].second;
		_orientations[i] = _corners[i]->normalize(rotation * corners[i].first);
	}
	_height = corners[0].second->_height + 1;
	
	// Rotate corners macro scale
	rotation.permuteCorners(_orientations[0], _orientations[1], _orientations[2], _orientations[3]);
	rotation.permuteCorners(_corners[0], _corners[1], _corners[2], _corners[3]);
	
	// Determine symmetry group
	
	// Update hash
	_hash = generateHash();
}

uint64 BoardNode::generateHash() const
{
	// Hash each cell
	uint64 tl = _corners[0]->hash() ^ _orientations[0].hash();
	uint64 tr = _corners[1]->hash() ^ _orientations[1].hash();
	uint64 bl = _corners[2]->hash() ^ _orientations[2].hash();
	uint64 br = _corners[3]->hash() ^ _orientations[3].hash();
	
	// Multiply each cell by a unique 64 bit prime
	tl *= 0xe4e5a4def3ae754fUL;
	tr *= 0xf0d56d32e10c8243UL;
	bl *= 0xfabfd7310ee77e0bUL;
	br *= 0xcafaf37f45c82715UL;
	
	// Xor them together
	tl ^= tr;
	bl ^= br;
	tl ^= bl;
	
	// Mix them some more
	tl = rotate_left(tl, 31);
	tl *= 0x4cf5ad432745937fUL;
	tl ^= tl >> 33;
	return tl;
}

BoardNode::OrientedBoardNode BoardNode::piece(uint n)
{
	assert(n < 9);
	assert(_height > 0);
	if(n == 0)
		//   X X - -
		//   X X - -
		//   - - - -
		//   - - - -
		return make_pair(_orientations[0], _corners[0]);
	if(n == 1)
		//   - - X X
		//   - - X X
		//   - - - -
		//   - - - -
		return make_pair(_orientations[1], _corners[1]);
	if(n == 2)
		//   - - - -
		//   - - - -
		//   X X - -
		//   X X - -
		return make_pair(_orientations[2], _corners[2]);
	if(n == 3)
		//   - - - -
		//   - - - -
		//   - - X X
		//   - - X X
		return make_pair(_orientations[3], _corners[3]);
	assert(_height > 1);
	if(n == 4)
		//   - X X -
		//   - X X -
		//   - - - -
		//   - - - -
		return get(
			subPiece(0, 1),
			subPiece(0, 2),
			subPiece(1, 1),
			subPiece(1, 2)
		);
	if(n == 5)
		//   - - - -
		//   X X - -
		//   X X - -
		//   - - - -
		return get(
			subPiece(1, 0),
			subPiece(1, 1),
			subPiece(2, 0),
			subPiece(2, 1)
		);
	if(n == 6)
		//   - - - -
		//   - - X X
		//   - - X X
		//   - - - -
		return get(
			subPiece(1, 2),
			subPiece(1, 3),
			subPiece(2, 2),
			subPiece(2, 3)
		);
	if(n == 7)
		//   - - - -
		//   - - - -
		//   - X X -
		//   - X X -
		return get(
			subPiece(2, 1),
			subPiece(2, 2),
			subPiece(3, 1),
			subPiece(3, 2)
		);
	if(n == 8)
		//   - - - -
		//   - X X -
		//   - X X -
		//   - - - -
		return get(
			subPiece(1, 1),
			subPiece(1, 2),
			subPiece(2, 1),
			subPiece(2, 2)
		);
	assert(false);
}

BoardNode::OrientedBoardNode BoardNode::subPiece(uint r, uint c)
{
	assert(_height > 0);
	assert(r < 4);
	assert(c < 4);
	uint outerCorner = 2 * (r / 2) + (c / 2);
	Rotation outerRotate = _orientations[outerCorner];
	BoardNode* outerNode = _corners[outerCorner];
	assert(outerNode != nullptr);
	r %= 2;
	c %= 2;
	outerRotate.inverted().transform(2, r, c);
	uint innerCorner = 2 * r + c;
	Rotation innerRotate = outerNode->_orientations[innerCorner];
	BoardNode* innerNode = outerNode->_corners[innerCorner];
	assert(innerNode != nullptr);
	return make_pair(outerRotate * innerRotate, innerNode);
}

void BoardNode::print(char* buffer, uint rowStride, Rotation rotation) const
{
	if(rowStride == 0)
		rowStride = size();
	if(_height == 0) {
		if(this == &_wall)
			buffer[0] = '#';
		else if(this == &_empty)
			buffer[0] = '.';
		else if(this == &_player)
			buffer[0] = 'X';
		else if(this == &_opponent)
			buffer[0] = 'O';
	} else {
		for(uint i = 0; i < 4; ++i) {
			uint col = i % 2;
			uint row = i / 2;
			rotation.transform(2, row, col);
			row *= size() / 2;
			col *= size() / 2;
			uint offset = col + (row * rowStride);
			_corners[i]->print(buffer + offset, rowStride, rotation * _orientations[i]);
		}
	}
}

void BoardNode::test()
{
	Board start;
	cerr << BoardNode::fragmentCount() << endl;
	
	BoardNode::dumpFragments();
	
	BoardNode::OrientedBoardNode obn = BoardNode::get(start);
	
	cerr << BoardNode::fragmentCount() << endl;
	BoardNode::dumpFragments();
	
	cerr << "Result: " << endl;
	cerr << obn << endl;
	
	cerr << "Subpieces: " << endl;
	for(uint i = 0; i < 4; ++i)
	for(uint j = 0; j < 4; ++j) {
		uint r = i;
		uint c = j;
		obn.first.transform(4, r, c);
		cerr << i << " " << j << " " << r << " " << c << endl;
		BoardNode::OrientedBoardNode piece = obn.second->subPiece(r, c);
		piece.first = obn.first * piece.first;
		cerr << piece << endl;
	}
	
	cerr << "Pieces: " << endl;
	for(uint i = 0; i < 9; ++i) {
		BoardNode::OrientedBoardNode piece = obn.second->piece(i);
		piece.first = obn.first * piece.first;
		cerr << piece << endl;
	}
	
	cerr << BoardNode::fragmentCount() << endl;
	BoardNode::dumpFragments();
}

