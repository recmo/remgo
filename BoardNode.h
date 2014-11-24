#pragma once
#include "Rotation.h"
#include "SymmetryGroup.h"
#include "Board.h"
#include <unordered_map>

class BoardNode {
public:
	typedef std::pair<Rotation, BoardNode*> OrientedBoardNode;
	static void test() funk;
	static void test(const Board& board) funk;
	static void initialize() funk;
	static void dumpFragments(std::ostream& out = std::cerr);
	static void dumpStats(std::ostream& out = std::cerr);
	static void dumpHisto(std::ostream& out = std::cerr);
	static uint fragmentCount() funk { return _fragments.size(); }
	static OrientedBoardNode get(const Board& board) funk;
	static OrientedBoardNode get(const Board& board, int x, int y, uint s) funk;
	static OrientedBoardNode get(const OrientedBoardNode corners[4]) funk;
	static OrientedBoardNode get(const OrientedBoardNode& tl, const OrientedBoardNode& tr, const OrientedBoardNode& bl, const OrientedBoardNode& br) funk;
	
	uint64 hash() const funk { return _hash; }
	uint height() const funk { return _height; }
	uint size() const funk { return 1 << _height; }
	uint visits() const funk { return _visits; }
	sint score() const funk { return _score; }
	double averageScore() const funk { return static_cast<double>(_score) / static_cast<double>(_visits); }
	void addRecursive(uint visits, sint score) funk;
	
	Board board(Rotation rotation, uint moveCount) const funk;
	
	OrientedBoardNode piece(uint n) funk;
	OrientedBoardNode subPiece(uint r, uint c) funk;
	
private:
	static uint cellIndex(const OrientedBoardNode& node);
	static bool _initialized;
	static std::unordered_map<uint64, BoardNode*> _fragments;
	static OrientedBoardNode _smallCache[4][4][4][4];
	static BoardNode _wall;
	static BoardNode _empty;
	static BoardNode _player;
	static BoardNode _opponent;
	friend std::ostream& operator<<(std::ostream& out, const BoardNode& boardNode) funk;
	friend std::ostream& operator<<(std::ostream& out, const OrientedBoardNode& boardNode) funk;
	
	BoardNode() funk;
	BoardNode(uint64 hash) funk;
	BoardNode(const OrientedBoardNode& tl, const OrientedBoardNode& tr, const OrientedBoardNode& bl, const OrientedBoardNode& br) funk;
	BoardNode(const BoardNode& copy) funk;
	~BoardNode() funk { }
	
	uint64 _hash;
	uint8 _height;
	SymmetryGroup _symmetries;
	Rotation _orientations[4];
	BoardNode* _corners[4];
	uint32 _visits;
	sint32 _score;
	
	void rotate(Rotation rotation) funk;
	Rotation cannonicalOrientate() funk;
	Rotation symmetryReduce(Rotation rotation) const funk { return _symmetries.normalize(rotation); }
	void updateHash() funk;
	void findSymmetryGroup() funk;
	void addRecursive(const OrientedBoardNode piece, uint visits, sint score) funk;
	
	void pieces(BoardMask& player, BoardMask& opponent, Rotation rotation, int x, int y) const funk;
	void print(char* buffer, uint rowStride = 0, Rotation rotation = Rotation::r0()) const funk;
};

std::ostream& operator<<(std::ostream& out, const BoardNode& boardNode) funk;
std::ostream& operator<<(std::ostream& out, const BoardNode::OrientedBoardNode& boardNode) funk;

// std::hash implementation
namespace std {
	template<> struct hash<BoardNode> {
		typedef BoardNode argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const {
			return s.hash();
		}
	};
}
