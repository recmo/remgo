#pragma once
#include "Rotation.h"
#include "SymmetryGroup.h"
#include "Board.h"
#include <unordered_map>

class BoardNode {
public:
	typedef std::pair<Rotation, BoardNode*> OrientedBoardNode;
	static void test();
	static void test(const Board& board);
	static void initialize();
	static void dumpFragments(std::ostream& out = std::cerr);
	static void dumpStats(std::ostream& out = std::cerr);
	static void dumpHisto(std::ostream& out = std::cerr);
	static uint fragmentCount() { return _fragments.size(); }
	static OrientedBoardNode get(const Board& board);
	static OrientedBoardNode get(const Board& board, int x, int y, uint s);
	static OrientedBoardNode get(const OrientedBoardNode corners[4]);
	static OrientedBoardNode get(const OrientedBoardNode& tl, const OrientedBoardNode& tr, const OrientedBoardNode& bl, const OrientedBoardNode& br);
	
	uint64 hash() const { return _hash; }
	uint height() const { return _height; }
	uint size() const { return 1 << _height; }
	uint visits() const { return _visits; }
	sint score() const { return _score; }
	double averageScore() const { return static_cast<double>(_score) / static_cast<double>(_visits); }
	void addRecursive(uint visits, sint score);
	void subRecursive();
	void unsubRecursive();
	
	Board board(Rotation rotation, uint moveCount) const;
	
	OrientedBoardNode piece(uint n);
	OrientedBoardNode subPiece(uint r, uint c);
	
private:
	static uint cellIndex(const OrientedBoardNode& node);
	static bool _initialized;
	static std::unordered_map<uint64, BoardNode*> _fragments;
	static OrientedBoardNode _smallCache[4][4][4][4];
	static BoardNode _wall;
	static BoardNode _empty;
	static BoardNode _player;
	static BoardNode _opponent;
	friend std::ostream& operator<<(std::ostream& out, const BoardNode& boardNode);
	friend std::ostream& operator<<(std::ostream& out, const OrientedBoardNode& boardNode);
	
	BoardNode();
	BoardNode(uint64 hash);
	BoardNode(const OrientedBoardNode& tl, const OrientedBoardNode& tr, const OrientedBoardNode& bl, const OrientedBoardNode& br);
	BoardNode(const BoardNode& copy);
	~BoardNode() { }
	
	uint64 _hash;
	uint8 _height;
	SymmetryGroup _symmetries;
	Rotation _orientations[4];
	BoardNode* _corners[4];
	uint32 _visits;
	sint32 _score;
	
	void rotate(Rotation rotation);
	Rotation cannonicalOrientate();
	Rotation symmetryReduce(Rotation rotation) const { return _symmetries.normalize(rotation); }
	void updateHash();
	void findSymmetryGroup();
	void addRecursive(const OrientedBoardNode piece, uint visits, sint score);
	
	void pieces(BoardMask& player, BoardMask& opponent, Rotation rotation, int x, int y) const;
	void print(char* buffer, uint rowStride = 0, Rotation rotation = Rotation::r0()) const;
};

std::ostream& operator<<(std::ostream& out, const BoardNode& boardNode);
std::ostream& operator<<(std::ostream& out, const BoardNode::OrientedBoardNode& boardNode);

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
