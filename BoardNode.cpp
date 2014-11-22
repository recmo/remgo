#include "BoardNode.h"

bool BoardNode::_initialized = false;
std::unordered_map<uint64, BoardNode*> BoardNode::_fragments;
BoardNode::OrientedBoardNode BoardNode::_smallCache[4][4][4][4];
BoardNode BoardNode::_wall(0xf07e84658f112f0fUL);
BoardNode BoardNode::_empty(0xd60d74b3eb29093dUL);
BoardNode BoardNode::_player(0x9faa2a2dd3178d69UL);

std::ostream& operator<<(std::ostream& out, const BoardNode& boardNode)
{
	out.width(16);
	out.fill('0');
	out << right << hex << boardNode.hash() << dec << " " << boardNode.height() << " " << boardNode.visits() << " " << boardNode.score() << endl;
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
	assert(!_initialized);
	
	// Initialize one by one pieces
	_fragments[_wall.hash()] = &_wall;
	_fragments[_empty.hash()] = &_empty;
	_fragments[_player.hash()] = &_player;
	_player._symmetries = SymmetryGroup::space();
	
	// Initialize two by two pieces cache
	std::array<OrientedBoardNode, 4> cells = {
		make_pair(Rotation(), &_empty),
		make_pair(Rotation(), &_wall),
		make_pair(Rotation(), &_player),
		make_pair(Rotation::pC(), &_player)
	};
	for(uint tl = 0; tl < 4; ++tl)
	for(uint tr = 0; tr < 4; ++tr)
	for(uint bl = 0; bl < 4; ++bl)
	for(uint br = 0; br < 4; ++br)
		_smallCache[tl][tr][bl][br] = get(cells[tl], cells[tr], cells[bl], cells[br]);
	
	// Flag initialized
	_initialized = true;
}

uint BoardNode::cellIndex(const BoardNode::OrientedBoardNode& node)
{
	if(node.second == &_empty)
		return 0;
	if(node.second == &_wall)
		return 1;
	if(node.second == &_player)
		if(!node.first.colourFlipped())
			return 2;
		else
			return 3;
	assert(false);
}

void BoardNode::dumpFragments(ostream& out)
{
	for(auto i: _fragments)
		out << *(i.second) << endl;
}

void BoardNode::dumpStats(ostream& out)
{
	for(auto i: _fragments) {
		BoardNode* node = i.second;
		if(node->height() == 0)
			continue;
		if(node->height() != 1)
			continue;
		out << *node << endl;
		out << node->height() << ", ";
		double score = node->averageScore();
		out << score << ", ";
		
		for(uint i = 0; i < 9; ++i) {
			if(node->height() > 1 || i < 4) {
				BoardNode::OrientedBoardNode obn = node->piece(i);
				double score = obn.first.colourFlipped() ? -obn.second->averageScore() : obn.second->averageScore();
				out << score;
			} else {
				out << "0";
			}
			if(i < 8)
				out << ", ";
			else
				out << endl;
		}
	}
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
		if(board.playerPieces().isSet(BoardPoint(y, x)))
			return make_pair(Rotation(), &_player);
		if(board.opponentPieces().isSet(BoardPoint(y, x)))
			return make_pair(Rotation::pC(), &_player);
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

BoardNode::OrientedBoardNode BoardNode::get(const BoardNode::OrientedBoardNode& tl, const BoardNode::OrientedBoardNode& tr, const BoardNode::OrientedBoardNode& bl, const BoardNode::OrientedBoardNode& br)
{
	// Use cache for 2x2 pieces
	if(_initialized && tl.second->height() == 0)
		return _smallCache[cellIndex(tl)][cellIndex(tr)][cellIndex(bl)][cellIndex(br)];
	
	// Find the cannonical orientation
	BoardNode orientable(tl, tr, bl, br);
	Rotation orientation = orientable.cannonicalOrientate();
	orientable.updateHash();
	
	// Now deduplicate the node
	BoardNode* node = _fragments[orientable.hash()];
	if(node == nullptr) {
		// Does not exist yet, create a new one
		node = new BoardNode(orientable);
		node->findSymmetryGroup();
		_fragments[node->hash()] = node;
	}
	
	return make_pair(node->symmetryReduce(orientation.inverted()), node);
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

BoardNode::BoardNode(const OrientedBoardNode& tl, const OrientedBoardNode& tr, const OrientedBoardNode& bl, const OrientedBoardNode& br)
: _hash(0)
, _height(tl.second->_height + 1)
, _symmetries()
, _orientations{tl.first, tr.first, bl.first, br.first}
, _corners{tl.second, tr.second, bl.second, br.second}
{
}

BoardNode::BoardNode(const BoardNode& copy)
: _hash(copy._hash)
, _height(copy._height)
, _symmetries(copy._symmetries)
, _orientations{copy._orientations[0], copy._orientations[1], copy._orientations[2], copy._orientations[3]}
, _corners{copy._corners[0], copy._corners[1], copy._corners[2], copy._corners[3]}
{
}

Board BoardNode::board(Rotation rotation, uint moveCount) const
{
	assert(_height == 4);
	BoardMask player;
	BoardMask opponent;
	
	pieces(player, opponent, rotation, -2, -2);
	
	if(moveCount & 1)
		return Board(opponent, player, moveCount);
	else
		return Board(player, opponent, moveCount);
}

void BoardNode::pieces(BoardMask& player, BoardMask& opponent, Rotation rotation, int row, int col) const
{
	if(_height == 0) {
		if(this != &_player)
			return;
		assert(row >= 0 && row < 11);
		assert(col >= 0 && col < 11);
		if(!rotation.colourFlipped())
			player.set(BoardPoint(row, col));
		else
			opponent.set(BoardPoint(row, col));
		return;
	}
	
	// Recurse
	for(uint i = 0; i < 4; ++i) {
		uint scol = i % 2;
		uint srow = i / 2;
		rotation.transform(2, srow, scol);
		srow *= size() / 2;
		scol *= size() / 2;
		_corners[i]->pieces(player, opponent, rotation * _orientations[i], row + srow, col + scol);
	}
}


void BoardNode::rotate(Rotation rotation)
{
	rotation.permuteCorners(_corners[0], _corners[1], _corners[2], _corners[3]);
	rotation.permuteCorners(_orientations[0], _orientations[1], _orientations[2], _orientations[3]);
	_orientations[0] = _corners[0]->symmetryReduce(rotation * _orientations[0]);
	_orientations[1] = _corners[1]->symmetryReduce(rotation * _orientations[1]);
	_orientations[2] = _corners[2]->symmetryReduce(rotation * _orientations[2]);
	_orientations[3] = _corners[3]->symmetryReduce(rotation * _orientations[3]);
	_hash = 0;
}

/// Rotates the board into it's unique canonical orientation
Rotation BoardNode::cannonicalOrientate()
{
	// Fall back to taking the lowest hash
	/// TODO: Something more efficient?
	Rotation current = Rotation::r0();
	Rotation lowest;
	uint64 lowestHash = std::numeric_limits<uint64>::max();
	for(Rotation r: Rotation::all) {
		rotate(r / current);
		current = r;
		updateHash();
		if(hash() < lowestHash) {
			lowest = r;
			lowestHash = hash();
		}
	}
	
	// Rotate to the lowest setting
	rotate(lowest / current);
	updateHash();
	assert(hash() == lowestHash);
	return lowest;
}

void BoardNode::findSymmetryGroup()
{
	uint16 symmetries = 0x0000;
	uint16 mask = 1;
	for(Rotation a: Rotation::all) {
		BoardNode copy(*this);
		copy.rotate(a);
		copy.updateHash();
		if(hash() == copy.hash())
			symmetries |= mask;
		mask <<= 1;
	}
	_symmetries = SymmetryGroup(symmetries);
}

void BoardNode::updateHash()
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
	_hash = tl;
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

void BoardNode::addRecursive(uint visits, sint score)
{
	assert(abs(score) <= visits);
	if(_visits > (1UL << 30)) {
		_visits >>= 1;
		_score >>= 1;
	}
	_visits += visits;
	_score += score;
	if(_height > 0)
		for(uint i = 0; i < 4; ++i)
			addRecursive(piece(i), visits, score);
	if(_height > 1)
		for(uint i = 4; i < 9; ++i)
			addRecursive(piece(i), visits, score);
}

void BoardNode::addRecursive(const BoardNode::OrientedBoardNode piece, uint visits, sint score)
{
	piece.second->addRecursive(visits, piece.first.colourFlipped() ? -score : score);
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
			buffer[0] = rotation.colourFlipped() ? 'O' : 'X';
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
	
	// Test for some random boards
	cerr << "Testing random boards" << endl;
	for(uint i = 0; i < 100000; ++i) {
		Board board;
		for(uint j = 0; j < 50; ++j) {
			board.playMove(board.randomMove());
			BoardNode::test(board);
		}
	}
	cerr << BoardNode::fragmentCount() << endl;
}


void BoardNode::test(const Board& board)
{
	BoardNode::OrientedBoardNode obn = BoardNode::get(board);
	Board recovered = obn.second->board(obn.first, board.moveCount());
	if(board != recovered) {
		cerr << board << endl;
		cerr << obn << endl;
		cerr << recovered << endl;
	}
	assert(board == recovered);
}

