#pragma once
#include "BoardPoint.h"

class BoardMask {
public:
	class Iterator; 
	const static BoardMask fullBoard;
	const static BoardMask borders[5];
	
	BoardMask(): _a(0), _b(0) { }
	BoardMask(const BoardMask& other): _a(other._a), _b(other._b) { }
	BoardMask(BoardPoint point);
	~BoardMask() { }
	operator bool() const { return !isEmpty(); }
	BoardMask& operator=(const BoardMask& other) { _a = other._a; _b = other._b; return *this; }
	bool operator==(const BoardMask& other) const { return _a == other._a && _b == other._b; }
	bool operator!=(const BoardMask& other) const { return !operator==(other); }
	BoardMask& operator&=(const BoardMask& other) { return operator=(operator&(other)); }
	BoardMask& operator|=(const BoardMask& other) { return operator=(operator|(other)); }
	BoardMask& operator-=(const BoardMask& other) { return operator=(operator-(other)); }
	BoardMask operator&(const BoardMask& other) const { return BoardMask(_a & other._a, _b & other._b); }
	BoardMask operator|(const BoardMask& other) const { return BoardMask(_a | other._a, _b | other._b); }
	BoardMask operator-(const BoardMask& other) const { return *this & (~other); }
	BoardMask operator~() const { return BoardMask(~_a, ~_b) & fullBoard; }
	BoardMask expanded() const;
	BoardMask connected(const BoardMask& seed) const;
	vector<BoardMask> groups() const;
	uint controlledCorners() const;
	BoardMask winningSet() const;
	BoardMask& invert() { return operator=(operator~()); }
	BoardMask& expand() { return operator=(expanded()); }
	BoardMask& clear() { return operator=(BoardMask()); }
	uint popcount() const { return ::popcount(_a) + ::popcount(_b); }
	bool isSet(BoardPoint point) const { return !(*this & BoardMask(point)).isEmpty(); }
	BoardMask& set(BoardPoint point) { return operator=(*this | BoardMask(point)); }
	BoardMask& clear(BoardPoint point) { return operator=(*this & ~BoardMask(point)); }
	bool isEmpty() const { return operator==(BoardMask()); }
	bool isValid() const { return operator==(*this & fullBoard); }
	BoardPoint firstPoint() const;
	BoardPoint randomPoint() const;
	Iterator itterator() const;
	
	uint64 a() const { return _a; }
	uint64 b() const { return _b; }
	
protected:
	BoardMask(uint64 a, uint64 b) :_a(a), _b(b) { }
	
protected:
	uint64 _a;
	uint64 _b;
};

class BoardMask::Iterator {
public:
	Iterator(const BoardMask& mask): _mask(mask), _point(_mask.firstPoint()) { }
	~Iterator() { }
	operator bool() const { return _mask._a | _mask._b; }
	Iterator& operator++() { _mask.clear(_point); _point = _mask.firstPoint(); return *this; }
	Iterator operator++(int) { Iterator tmp(*this); operator++(); return tmp; }
	const BoardPoint& operator*() const { return _point; }
	const BoardPoint* operator->() const { return &_point; }
	
private:
	BoardMask _mask;
	BoardPoint _point;
};
