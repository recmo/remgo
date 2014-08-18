#pragma once
#include "BoardPoint.h"

class BoardMask {
public:
	class Iterator; 
	const static BoardMask fullBoard;
	
	BoardMask(): _mask(0) { }
	BoardMask(const BoardMask& other): _mask(other._mask) { }
	BoardMask(BoardPoint point);
	~BoardMask() { }
	operator bool() const { return !isEmpty(); }
	BoardMask& operator=(const BoardMask& other) { _mask = other._mask;return *this; }
	bool operator==(const BoardMask& other) const { return _mask == other._mask; }
	bool operator!=(const BoardMask& other) const { return !operator==(other); }
	BoardMask& operator&=(const BoardMask& other) { return operator=(operator&(other)); }
	BoardMask& operator|=(const BoardMask& other) { return operator=(operator|(other)); }
	BoardMask& operator-=(const BoardMask& other) { return operator=(operator-(other)); }
	BoardMask operator&(const BoardMask& other) const { return BoardMask(_mask & other._mask); }
	BoardMask operator|(const BoardMask& other) const { return BoardMask(_mask | other._mask); }
	BoardMask operator-(const BoardMask& other) const { return *this & (~other); }
	BoardMask operator~() const { return BoardMask(~_mask) & fullBoard; }
	BoardMask expanded() const;
	BoardMask connected(const BoardMask& seed) const;
	vector<BoardMask> groups() const;
	BoardMask& invert() { return operator=(operator~()); }
	BoardMask& expand() { return operator=(expanded()); }
	BoardMask& clear() { return operator=(BoardMask()); }
	uint popcount() const { return ::popcount(_mask); }
	bool isSet(BoardPoint point) const { return !(*this & BoardMask(point)).isEmpty(); }
	BoardMask& set(BoardPoint point) { return operator=(*this | BoardMask(point)); }
	BoardMask& clear(BoardPoint point) { return operator=(*this & ~BoardMask(point)); }
	bool isEmpty() const { return operator==(BoardMask()); }
	bool isValid() const { return operator==(*this & fullBoard); }
	BoardPoint firstPoint() const;
	BoardPoint randomPoint() const;
	Iterator itterator() const;
	uint128 mask() const { return _mask; }
	
	Iterator begin() const;
	Iterator end() const;

protected:
	uint128 _mask;
	BoardMask(uint128 mask): _mask(mask) { }
	BoardMask& setFullBoard();
};

std::ostream& operator<<(std::ostream& out, const BoardMask& mask);

class BoardMask::Iterator {
public:
	Iterator(const BoardMask& mask): _mask(mask), _point(_mask.firstPoint()) { }
	~Iterator() { }
	operator bool() const { return _mask; }
	Iterator& operator++() { _mask.clear(_point); _point = _mask.firstPoint(); return *this; }
	Iterator operator++(int) { Iterator tmp(*this); operator++(); return tmp; }
	const BoardPoint& operator*() const { return _point; }
	const BoardPoint* operator->() const { return &_point; }
	bool operator!= (const Iterator& other) const { return _mask != other._mask; }
	bool operator== (const Iterator& other) const { return _mask == other._mask; }
	
private:
	BoardMask _mask;
	BoardPoint _point;
};

inline BoardMask::Iterator BoardMask::begin() const { return Iterator(*this); }

inline BoardMask::Iterator BoardMask::end() const { return Iterator(BoardMask()); }

