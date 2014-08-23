#include "Board.h"
#include "Move.h"
#include "Random.h"
#include "DijkstraHeuristic.h"

std::ostream& operator<<(std::ostream& out, const Board& board)
{
	for(uint row = 0; row < 11; ++row) {
		for(uint col = 0; col < 11; ++col) {
			BoardPoint i(10 - row, col);
			bool white = board.white().isSet(i);
			bool black = board.black().isSet(i);
			if(!white && !black)
				out << ".";
			if(white && !black)
				out << "W";
			if(!white && black)
				out << "B";
			if(white && black)
				out << "!";
		}
		out << endl;
	}
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
		if(m.isValid()) {
			
			assert(isValidMove(m));
			return m;
		}
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

sint Board::heuristicStrength() const
{
	DijkstraHeuristic playerHeuristic(playerPieces(), free());
	DijkstraHeuristic opponentHeuristic(opponentPieces(), free());
	return playerHeuristic.dijkstra() - opponentHeuristic.dijkstra();
}

vector<Move> Board::sortedMoves() const
{
	vector<Move> moves = validMoves();
	vector<pair<sint, Move>> sorted;
	sorted.reserve(moves.size());
	for(Move move: moves) {
		// Calculate strength _after_ the move, so lower is better
		sint strength = Board(*this).playMove(move).heuristicStrength();
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
	return sortedMoves().front();
}


// TODO: Heuristic: What is the minimal number of stones required to connect all connectable units?
// Subtract players count from opponents to get a basic heuristic of the positions strength.

// We can also do move ordering by preferring moves that are on the MST
// We can prefer moves that would block the opponent MST (unless this makes opponent disconnected)
