
#include "MoveHeuristic.h"
#include "MoveHeuristic.conf.h"


MoveHeuristic::MoveHeuristic(const Board& board)
: _board(board)
{
	Dijkstra player(board.playerPieces(), board.free());
	player.dijkstra();
	_playerMst = player.minimalSpanningTree();
	_playerRedundant = player.treeRedudancy();
	
	Dijkstra opponent(board.opponentPieces(), board.free());
	opponent.dijkstra();
	_opponentMst = opponent.minimalSpanningTree();
	_opponentRedundant = opponent.treeRedudancy();
	
	
	BoardMask neighbors = board.playerPieces().expanded();
	_oldNeighborScore  = (neighbors & board.free()).popcount() * freeNeighborScore;
	_oldNeighborScore += (neighbors & board.opponentPieces()).popcount() * opponentNeighborScore;
}

float MoveHeuristic::evaluate(Move move)
{
	const BoardPoint from = move.from();
	const BoardPoint to = move.to();
	
	// A piece is a leaf if it is not part of the non-redundant MST
	const bool leafPiece = !_playerMst.isSet(from) || _playerRedundant.isSet(from);
	
	// We can block or hinder the opponents MST
	const bool opponentBlock = _opponentMst.isSet(to) && !_opponentRedundant.isSet(to);
	const bool opponentHinder = _opponentRedundant.isSet(to);
	
	// We can improve our own MST
	const bool improveMST = _playerMst.isSet(to);
	
	BoardMask neighbors = _board.playerPieces();
	neighbors.clear(from);
	neighbors.set(to);
	neighbors.expand();
	float neighborScore  = (neighbors & _board.free()).popcount() * freeNeighborScore;
	neighborScore += (neighbors & _board.opponentPieces()).popcount() * opponentNeighborScore;
	neighborScore -= _oldNeighborScore;
	
	float score = neighborScore;
	if(!leafPiece)
		score += nonLeafPieceScore;
	if(opponentBlock)
		score += opponentBlockScore;
	if(opponentHinder)
		score += opponentHinderScore;
	if(improveMST)
		score += improveMSTScore;
	return score;
}
