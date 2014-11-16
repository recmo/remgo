#pragma once
#include "Rotation.h"
#include "SymmetryGroup.h"
#include "Board.h"
#include <unordered_map>

class BoardNode {
public:
	static void test() funk;
	static void initialize() funk;
	static void dumpFragments(std::ostream& out = std::cerr);
	static uint fragmentCount() funk { return _fragments.size(); }
	
	typedef std::pair<Rotation, BoardNode*> OrientedBoardNode;
	
	static OrientedBoardNode get(const Board& board) funk;
	static OrientedBoardNode get(const Board& board, int x, int y, uint s) funk;
	static OrientedBoardNode get(const OrientedBoardNode corners[4]) funk;
	static OrientedBoardNode get(const OrientedBoardNode& tl, const OrientedBoardNode& tr, const OrientedBoardNode& bl, const OrientedBoardNode& br) funk;
	
	uint64 hash() const funk { return _hash; }
	uint height() const funk { return _height; }
	uint size() const funk { return 1 << _height; }
	
	Rotation normalize(Rotation rotation) const funk { return _symmetries.normalize(rotation); }
	
	OrientedBoardNode piece(uint n) funk;
	OrientedBoardNode subPiece(uint r, uint c) funk;
	
private:
	static std::unordered_map<uint64, BoardNode*> _fragments;
	static BoardNode _wall;
	static BoardNode _empty;
	static BoardNode _player;
	static BoardNode _opponent;
	friend std::ostream& operator<<(std::ostream& out, const BoardNode& boardNode) funk;
	friend std::ostream& operator<<(std::ostream& out, const OrientedBoardNode& boardNode) funk;
	
	BoardNode() funk;
	BoardNode(uint64 hash) funk;
	BoardNode(const OrientedBoardNode corners[4], Rotation rotation = Rotation::r0()) funk;
	~BoardNode() funk { }
	
	uint64 _hash;
	uint8 _height;
	SymmetryGroup _symmetries;
	Rotation _orientations[4];
	BoardNode* _corners[4];
	
	uint64 generateHash() const funk;
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
