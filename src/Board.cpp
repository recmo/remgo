#include "Board.h"
#include "Move.h"
#include "Random.h"
#include "MoveHeuristic.h"

wostream& operator<<(wostream& out, const Board& board)
{
	// Unicode box drawing:
	// ┌─┬─┐  250C 2500 252C 2500 2510
	// ├─┼─┤  251C 2500 253C 2500 2524
	// └─┴─┘  2514 2500 2534 2500 2518
	// ┌─W─┐
	// B─╬─┤  256C
	// └─┴─┘
	const wchar_t space = 0x2500;
	const wchar_t hoshi = 0x256C;
	const wchar_t white = L'W';
	const wchar_t black = L'B';
	const wchar_t error = L'E';
	const wchar_t box[3][3] = {
		{0x250C, 0x252C, 0x2510},
		{0x251C, 0x253C, 0x2524},
		{0x2514, 0x2534, 0x2518}
	};
	const int first = 0;
	const int last = BoardPoint::size - 1;
	for(int row = last; row >= first; --row) {
		const int rowbox = (row == first) ? 2 : ((row == last) ? 0 : 1);
		const bool rowhoshi = (row == 3 || row == 9 || row == 15);
		out << dec << setw(2) << setfill(L' ') << (row + 1) << " ";
		for(int col = first; col <= last; ++col) {
			const int colbox = (col == first) ? 0 : ((col == last) ? 2 : 1);
			const bool colhoshi = (col == 3 || col == 9 || col == 15);
			const BoardPoint pos(row, col);
			const bool isWhite = board.white().isSet(pos);
			const bool isBlack = board.black().isSet(pos);
			if(isWhite  && !isBlack)
				out << white;
			else if(!isWhite  && isBlack)
				out << black;
			else if(isWhite  && isBlack)
				out << error;
			else if (rowhoshi && colhoshi)
				out << hoshi;
			else
				out << box[rowbox][colbox];
			if(col < last)
				out << space;
		}
		out << endl;
	}
	out << "   A B C D E F G H J K L M N O P Q R S T" << endl;
	if(board.player() == Board::White)
		out << "White to play" << endl;
	else
		out << "Black to play" << endl;
	
	out << "Board(" << board.white().mask() << ", " << board.black().mask() << ", " << board.moveCount() << ")" << endl;
	
	return out;
}

Board::Board()
: _white()
, _black()
, _moveCount(0)
{
	for(uint r = 0; r < 11; r += 2)
	for(uint c = 1; c < 11; c += 2)
		_white.set(BoardPoint(r, c));
	for(uint r = 1; r < 11; r += 2)
	for(uint c = 0; c < 11; c += 2)
		_black.set(BoardPoint(r, c));
}

Board::Board(BoardMask white, BoardMask black, uint moveCount)
: _white(white)
, _black(black)
, _moveCount(moveCount)
{
}

vector<Move> Board::validMoves(BoardPoint piece) const
{
	vector<Move> result;
	
	const BoardMask pp = playerPieces();
	const BoardMask empty = free();
	assert(pp.isSet(piece));
	
	BoardMask group = playerPieces().connected(piece);
	BoardMask nonGroup = playerPieces() - group;
	if(nonGroup.isEmpty()) {
		// Single big group, no valid moves left, we have won!
		return result;
	}
	
	// Find the nearest units
	BoardMask nearest;
	BoardMask frontier = group;
	do {
		BoardMask oldFrontier = frontier;
		frontier.expand();
		nearest = frontier & nonGroup;
		frontier &= empty;
		
		// No paths to other units
		if(!nearest && frontier == oldFrontier) {
			return result;
		}
	} while(!nearest);
	
	// Find the positions that decrease the distance towards them
	BoardMask boundary = group.expanded() & empty;
	BoardMask decreasers;
	frontier = nearest;
	do {
		frontier.expand();
		frontier &= empty;
		decreasers = frontier & boundary;
	} while(!decreasers);
	
	// For each distance decreasing destination
	for(BoardPoint to: decreasers) {
		// Make sure we are not splitting up the group with this move
		BoardMask newGroup = group;
		newGroup.clear(piece);
		newGroup.set(to);
		if(newGroup.connected(to) != newGroup)
			continue;
		
		// We have a valid move!
		result.push_back(Move(piece, to));
	}
	return result;
}

vector<Move> Board::validMoves() const
{
	vector<Move> result;
	for(BoardPoint p: playerPieces()) {
		for(Move m: validMoves(p)) {
			result.push_back(m);
		}
	}
	return result;
}

bool Board::isValidMove(Move move) const
{
	vector<Move> moves = validMoves();
	return find(moves.begin(), moves.end(), move) != moves.end();
}


Move Board::randomMove(BoardPoint piece) const
{
	const BoardMask pp = playerPieces();
	const BoardMask empty = free();
	assert(pp.isSet(piece));
	
	BoardMask group = playerPieces().connected(piece);
	BoardMask nonGroup = playerPieces() - group;
	if(nonGroup.isEmpty()) {
		// Single big group, no valid moves left, we have won!
		return Move();
	}
	
	// Find the nearest units
	BoardMask nearest;
	BoardMask frontier = group;
	do {
		BoardMask oldFrontier = frontier;
		frontier.expand();
		nearest = frontier & nonGroup;
		frontier &= empty;
		
		// No paths to other units
		if(!nearest && frontier == oldFrontier)
			return Move();
	} while(!nearest);
	
	
	
	// Find the positions that decrease the distance towards them
	BoardMask boundary = group.expanded() & empty;
	BoardMask decreasers;
	frontier = nearest;
	do {
		frontier.expand();
		frontier &= empty;
		decreasers = frontier & boundary;
	} while(!decreasers);
	
	
	// For each distance decreasing destination
	while(!decreasers.isEmpty()) {
		// Pick a move and make sure we are not splitting up the group with this move
		BoardPoint to = decreasers.randomPoint();
		BoardMask newGroup = group;
		newGroup.clear(piece);
		newGroup.set(to);
		if(newGroup.connected(to) == newGroup)
			return Move(piece, to);
		decreasers.clear(to);
	}
	
	return Move();
}

Move Board::randomMove() const
{
	// Accurate random move
	//vector<Move> moves = validMoves();
	//return moves[entropy(moves.size())];
	
	// This one is fast, but biased:
	BoardMask pp = playerPieces();
	do {
		BoardPoint p = pp.randomPoint();
		Move m = randomMove(p);
		if(m.isValid())
			return m;
		pp.clear(p);
	} while(pp);
	return Move();
}

Board& Board::playMove(Move move)
{
	// Verify the move correctness
	// assert(isValidMove(move));
	
	// Placement move
	if(player() == White) {
		assert(_white.isSet(move.from()));
		assert(free().isSet(move.to()));
		_white.clear(move.from());
		_white.set(move.to());
	} else {
		assert(_black.isSet(move.from()));
		assert(free().isSet(move.to()));
		_black.clear(move.from());
		_black.set(move.to());
	}
	
	// Increase move counter
	++_moveCount;
	return *this;
}

vector<Move> Board::sortedMoves() const
{
	MoveHeuristic heuristic(*this);
	
	vector<Move> moves = validMoves();
	vector<pair<float, Move>> sorted;
	sorted.reserve(moves.size());
	for(Move move: moves) {
		// Estimate move strength
		float strength = heuristic.evaluate(move);
		
		sorted.push_back(make_pair(strength, move));
	}
	sort(sorted.begin(), sorted.end());
	reverse(sorted.begin(), sorted.end());
	
	moves.clear();
	for(auto p: sorted) {
		moves.push_back(p.second);
	}
	return moves;
}

Move Board::heuristicMove() const
{
	vector<Move> sorted = sortedMoves();
	if(sorted.empty())
		return Move();
	return sorted.front();
}


// TODO: Heuristic: What is the minimal number of stones required to connect all connectable units?
// Subtract players count from opponents to get a basic heuristic of the positions strength.

// We can also do move ordering by preferring moves that are on the MST
// We can prefer moves that would block the opponent MST (unless this makes opponent disconnected)
