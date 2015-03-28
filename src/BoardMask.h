#pragma once
#include "BoardPoint.h"

class BoardMask {
public:
	static void initialize();
	static BoardMask masks[BoardPoint::numPositions + 1] aligned;
	
	class Iterator; 
	const static BoardMask fullBoard;
	
	BoardMask(): _mask() { }
	BoardMask(const BoardMask& other): _mask(other._mask) { }
	explicit BoardMask(BoardPoint point);
	~BoardMask() { }
	operator bool() const { return !isEmpty(); }
	BoardMask& operator=(const BoardMask& other) { _mask = other._mask; return *this; }
	bool operator==(const BoardMask& other) const { return _mask == other._mask; }
	bool operator!=(const BoardMask& other) const { return !operator==(other); }
	BoardMask& operator&=(const BoardMask& other) { return operator=(operator&(other)); }
	BoardMask& operator|=(const BoardMask& other) { return operator=(operator|(other)); }
	BoardMask& operator-=(const BoardMask& other) { return operator=(operator-(other)); }
	BoardMask operator&(const BoardMask& other) const { return BoardMask(_mask & other._mask); }
	BoardMask operator|(const BoardMask& other) const { return BoardMask(_mask | other._mask); }
	BoardMask operator-(const BoardMask& other) const { return BoardMask(_mask - other._mask); }
	BoardMask operator~() const { return fullBoard - *this; }
	BoardMask expanded() const;
	BoardMask connected(BoardPoint seed) const { return connected(BoardMask::masks[seed.position()]); }
	BoardMask connected(const BoardMask& seed) const;
	vector<BoardMask> groups() const;
	BoardMask& invert() { return operator=(operator~()); }
	BoardMask& expand() { return operator=(expanded()); }
	BoardMask& clear() { _mask.clear(); return *this; }
	uint popcount() const { return ::popcount(_mask); }
	bool isSet(BoardPoint point) const { return !(*this & BoardMask::masks[point.position()]).isEmpty(); }
	BoardMask& set(BoardPoint point) { return operator=(*this | BoardMask::masks[point.position()]); }
	BoardMask& clear(BoardPoint point) { return operator=(*this - BoardMask::masks[point.position()]); }
	bool isEmpty() const { return mask().isZero(); }
	bool isValid() const { return operator==(*this & fullBoard); }
	BoardPoint firstPoint() const;
	BoardPoint randomPoint() const;
	Iterator itterator() const;
	uint128 mask() const { return _mask; }
	
	Iterator begin() const;
	Iterator end() const;
	BoardMask(uint128 mask): _mask(mask) { }

protected:
	uint128 _mask aligned;
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
