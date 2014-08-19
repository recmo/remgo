#pragma once
#include "BoardPoint.h"

class BoardMask {
public:
	class Iterator; 
	const static BoardMask fullBoard;
	
	BoardMask() funk: _mask(0) { }
	BoardMask(const BoardMask& other) funk: _mask(other._mask) { }
	explicit BoardMask(BoardPoint point) funk;
	~BoardMask() funk { }
	operator bool() const funk { return !isEmpty(); }
	BoardMask& operator=(const BoardMask& other) funk { _mask = other._mask;return *this; }
	bool operator==(const BoardMask& other) const funk { return _mask == other._mask; }
	bool operator!=(const BoardMask& other) const funk { return !operator==(other); }
	BoardMask& operator&=(const BoardMask& other) funk { return operator=(operator&(other)); }
	BoardMask& operator|=(const BoardMask& other) funk { return operator=(operator|(other)); }
	BoardMask& operator-=(const BoardMask& other) funk { return operator=(operator-(other)); }
	BoardMask operator&(const BoardMask& other) const funk { return BoardMask(_mask & other._mask); }
	BoardMask operator|(const BoardMask& other) const funk { return BoardMask(_mask | other._mask); }
	BoardMask operator-(const BoardMask& other) const funk { return *this & (~other); }
	BoardMask operator~() const funk { return BoardMask(~_mask) & fullBoard; }
	BoardMask expanded() const funk;
	BoardMask connected(BoardPoint seed) const funk { return connected(BoardMask(seed)); }
	BoardMask connected(const BoardMask& seed) const funk;
	vector<BoardMask> groups() const funk;
	BoardMask& invert() funk { return operator=(operator~()); }
	BoardMask& expand() funk { return operator=(expanded()); }
	BoardMask& clear() funk { return operator=(BoardMask()); }
	uint popcount() const funk { return ::popcount(_mask); }
	bool isSet(BoardPoint point) const funk { return !(*this & BoardMask(point)).isEmpty(); }
	BoardMask& set(BoardPoint point) funk { return operator=(*this | BoardMask(point)); }
	BoardMask& clear(BoardPoint point) funk { return operator=(*this & ~BoardMask(point)); }
	bool isEmpty() const funk { return operator==(BoardMask()); }
	bool isValid() const funk { return operator==(*this & fullBoard); }
	BoardPoint firstPoint() const funk;
	BoardPoint randomPoint() const funk;
	Iterator itterator() const funk;
	uint128 mask() const funk { return _mask; }
	
	Iterator begin() const funk;
	Iterator end() const funk;
	explicit BoardMask(uint128 mask) funk: _mask(mask) { }

protected:
	uint128 _mask;
	BoardMask& setFullBoard() funk;
};

std::ostream& operator<<(std::ostream& out, const BoardMask& mask);

class BoardMask::Iterator {
public:
	Iterator(const BoardMask& mask) funk: _mask(mask), _point(_mask.firstPoint()) { }
	~Iterator() funk { }
	operator bool() const funk { return _mask; }
	Iterator& operator++() funk { _mask.clear(_point); _point = _mask.firstPoint(); return *this; }
	Iterator operator++(int) funk { Iterator tmp(*this); operator++(); return tmp; }
	const BoardPoint& operator*() const funk { return _point; }
	const BoardPoint* operator->() const funk { return &_point; }
	bool operator!= (const Iterator& other) const funk { return _mask != other._mask; }
	bool operator== (const Iterator& other) const funk { return _mask == other._mask; }
	
private:
	BoardMask _mask;
	BoardPoint _point;
};

inline BoardMask::Iterator BoardMask::begin() const { return Iterator(*this); }

inline BoardMask::Iterator BoardMask::end() const { return Iterator(BoardMask()); }

inline BoardMask BoardMask::expanded() const
{
	// Flanks, all non border positions
	const uint128 right = const128(0x1ffffffffffffffUL, 0xfffffffffffff800UL);
	const uint128 left  = const128(0x0003fffffffffffUL, 0xffffffffffffffffUL);
	const uint128 upper = const128(0x1ffbff7feffdffbUL, 0xff7feffdffbff7feUL);
	const uint128 lower = const128(0x0ffdffbff7feffdUL, 0xffbff7feffdffbffUL);
	uint128 r = _mask;
	r |= (_mask & lower) << 1;
	r |= (_mask & upper) >> 1;
	r |= (_mask & right) >> 11;
	r |= (_mask & left ) << 11;
	return BoardMask(r);
}
