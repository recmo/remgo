#include "Board.h"
#include "Move.h"
#include "Random.h"

std::ostream& operator<<(std::ostream& out, const Board& board)
{
	for(uint i = 1; i <= 106; ++i) {
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
	return out;
}

void Board::playMove(Move move)
{
	// Increase move counter
	++_moveCount;
	
	// Swap move
	if(move == Move::Swap) {
		assert(_moveCount == 2);
		swap(_black, _white);
		return;
	}
	
	// Placement move
	assert(!_black.isSet(move));
	assert(!_white.isSet(move));
	if(_moveCount & 1) {
		_black.set(move);
	} else {
		_white.set(move);
	}
}

bool Board::gameOver() const
{
	if(!nonSwapMoves())
		return true;
	if(_white.controlledCorners() >= 3)
		return true;
	if(_black.controlledCorners() >= 3)
		return true;
	return false;
}

uint Board::winner() const
{
	if(_white.controlledCorners() >= 3)
		return 1;
	if(_black.controlledCorners() >= 3)
		return 2;
	return 0;
}

// Connects bamboo bridges randomly
void Board::bambooBridges()
{
	BoardMask free = ~(_black | _white);
	
	// Find groups and extensions
	vector<BoardMask> whiteGroups = _white.groups();
	vector<BoardMask> whiteExtensions;
	for(const BoardMask& g: whiteGroups)
		whiteExtensions.push_back(g.expanded());
	vector<BoardMask> blackGroups = _black.groups();
	vector<BoardMask> blackExtensions;
	for(const BoardMask& g: blackGroups)
		blackExtensions.push_back(g.expanded());
	
	// Randomly play black or white
	uint whiteIndex = 0;
	uint blackIndex = 0;
	while((whiteIndex < whiteGroups.size()) || (blackIndex < blackGroups.size())) {
		uint i;
		BoardMask group;
		BoardMask* player = nullptr;
		BoardMask* opponent = nullptr;
		vector<BoardMask>* self = nullptr;
		if(whiteIndex != whiteGroups.size() && (blackIndex == blackGroups.size() || entropy(2))) {
			i = whiteIndex++;
			group = whiteGroups[i];
			player = &_white;
			opponent = &_black;
			self = &whiteExtensions;
		} else {
			i = blackIndex++;
			group = blackGroups[i];
			player = &_black;
			opponent = &_white;
			self = &blackExtensions;
		}
		BoardMask extension = self->at(i);
		
		// Borders
		for(uint j = 0; j < 5; j++) {
			BoardMask border = BoardMask::borders[j];
			if(group & border)
				continue; // Already connected to border
			BoardMask bamboo = extension & border & free;
			if(bamboo.popcount() == 2) {
				BoardPoint p = bamboo.randomPoint();
				player->set(p);
				*opponent |= bamboo - *player;
				free -= bamboo;
			}
		}
		
		// Other groups
		for(uint j = 0; j < i; j++) {
			BoardMask bamboo = extension & self->at(j) & free;
			if(bamboo.popcount() == 2) {
				BoardPoint p = bamboo.randomPoint();
				player->set(p);
				*opponent |= bamboo - *player;
				free -= bamboo;
			}
		}
	}
}

void Board::randomFillUp()
{
	BoardMask free = ~(_black | _white);
	uint whiteStones = 53 - _black.popcount();
	uint blackStones = 53 - _white.popcount();
	
	// Fill up with equal amounts of stones
	for(auto i = free.itterator(); i; i++) {
		if(blackOrWhite(blackStones, whiteStones)) {
			_white.set(*i);
			whiteStones--;
		} else {
			_black.set(*i);
			blackStones--;
		}
	}
	assert(whiteStones == 0 && blackStones == 0);
}
