#pragma once
#include "BoardPoint.h"

/// The full Go board has 19 × 19 = 361 positions.  19 is a terribly size to
/// pack for computers.

/// Bitpacking: 19 rows of 19 bits + 18 guard bits = 379 bits. Rounding this
/// to 384 bits gives 48 bytes for a board mask.

/// uint128:
/// There are several ways of packing this in uint128s.
/// Three uint128 can fit the whole board including guard bits, but some rows
/// will be split over two uint128s. This requires 48 bytes per mask.
/// One 128 bit uint can contain 6 rows leaving 14 guard bits.
/// Three uint128s will contain 18 rows. The remaining row can be stored in
/// a 
/// This means four uint128s will be needed. But if we use four anyway we can
/// also store only 5 rows per uint128 and have 33 guard bits.

/// uint256:
/// We can store 13 rows in an uint256 leaving only 9 guard bits.
/// The remaining six rows could be stored in a uint128.
/// Taking

/// Storing two board masks for the both kinds of stones takes 2 * 48 = 96 bytes.
/// Storing board state tertiary, we can fit one row in a uint32.
/// 3¹⁹ = 1162261467
/// 2³⁰ < 3¹⁹ < 2³¹
/// A uint32 thus nicely contains a row. Storing board state this way takes
/// 19 uint32s or 76 bytes. Accessing positions will require a lot of
/// multiplication, division and modulo operations however.

class BoardMask {
public:
	typedef array<uint128, 3> Bits;
	class Iterator; 
	const static BoardMask fullBoard;
	const static BoardMask topEdge;
	const static BoardMask bottomEdge;
	const static BoardMask leftEdge;
	const static BoardMask rightEdge;
	
	inline BoardMask();
	BoardMask(const BoardMask& other): _mask(other._mask) { }
	explicit BoardMask(BoardPoint point): BoardMask() { set(point); }
	~BoardMask() { }
	BoardMask copy() const { return BoardMask(*this); }
	operator bool() const { return !isEmpty(); }
	BoardMask& operator=(const BoardMask& other) { _mask = other._mask; return *this; }
	bool operator==(const BoardMask& other) const { return _mask == other._mask; }
	bool operator!=(const BoardMask& other) const { return !operator==(other); }
	inline BoardMask& operator&=(const BoardMask& other);
	inline BoardMask& operator|=(const BoardMask& other);
	inline BoardMask& operator-=(const BoardMask& other);
	inline BoardMask operator&(const BoardMask& other) const;
	inline BoardMask operator|(const BoardMask& other) const;
	inline BoardMask operator-(const BoardMask& other) const;
	BoardMask operator~() const { return fullBoard - *this; }
	BoardMask expanded() const;
	BoardMask connected(BoardPoint seed) const { return connected(BoardMask(seed)); }
	BoardMask connected(const BoardMask& seed) const;
	BoardMask up() const;
	BoardMask down() const;
	BoardMask left() const;
	BoardMask right() const;
	vector<BoardMask> groups() const;
	BoardMask& invert() { return operator=(operator~()); }
	BoardMask& expand() { return operator=(expanded()); }
	BoardMask& clear() { return operator=(BoardMask()); }
	inline uint popcount() const;
	inline bool isSet(BoardPoint point) const;
	inline BoardMask& set(BoardPoint point);
	inline BoardMask& clear(BoardPoint point);
	inline bool isEmpty() const;
	bool isValid() const { return operator==(*this & fullBoard); }
	BoardPoint firstPoint() const;
	BoardPoint nthPoint(uint n) const;
	BoardPoint randomPoint() const;
	Iterator itterator() const;
	
	Iterator begin() const;
	Iterator end() const;

protected:
	friend wostream& operator<<(wostream& out, const BoardMask& mask);
	static BoardMask createFullBoard();
	static BoardMask createTopEdge();
	static BoardMask createBottomEdge();
	static BoardMask createLeftEdge();
	static BoardMask createRightEdge();
	Bits _mask aligned;
};

wostream& operator<<(wostream& out, const BoardMask& mask);

BoardMask::BoardMask()
: _mask()
{
	_mask[0] = uint128(0);
	_mask[1] = uint128(0);
	_mask[2] = uint128(0);
}

BoardMask& BoardMask::operator&=(const BoardMask& other)
{
	_mask[0] &= other._mask[0];
	_mask[1] &= other._mask[1];
	_mask[2] &= other._mask[2];
	return *this;
}

BoardMask& BoardMask::operator|=(const BoardMask& other)
{
	_mask[0] |= other._mask[0];
	_mask[1] |= other._mask[1];
	_mask[2] |= other._mask[2];
	return *this;
}

BoardMask& BoardMask::operator-=(const BoardMask& other)
{
	_mask[0] &= ~(other._mask[0]);
	_mask[1] &= ~(other._mask[1]);
	_mask[2] &= ~(other._mask[2]);
	return *this;
}

BoardMask BoardMask::operator&(const BoardMask& other) const
{
	BoardMask r(*this);
	return r &= other;
}

BoardMask BoardMask::operator|(const BoardMask& other) const
{
	BoardMask r(*this);
	return r |= other;
}

BoardMask BoardMask::operator-(const BoardMask& other) const
{
	BoardMask r(*this);
	return r -= other;
}

uint BoardMask::popcount() const
{
	return ::popcount(_mask[0]) + ::popcount(_mask[1]) + ::popcount(_mask[2]);
}

bool BoardMask::isEmpty() const
{
	return (_mask[0] | _mask[1] | _mask[2]) == uint128(0);
}

bool BoardMask::isSet(BoardPoint point) const
{
	assert(point.isValid());
	const uint index = point.position();
	const uint mask = index / 128;
	const uint pos = index % 128;
	const uint128 bit = uint128(1) << pos;
	return (_mask[mask] & bit) != uint128(0);
}

BoardMask& BoardMask::set(BoardPoint point)
{
	assert(point.isValid());
	const uint index = point.position();
	const uint mask = index / 128;
	const uint pos = index % 128;
	const uint128 bit = uint128(1) << pos;
	_mask[mask] |= bit;
	return *this;
}

BoardMask& BoardMask::clear(BoardPoint point)
{
	assert(point.isValid());
	const uint index = point.position();
	const uint mask = index / 128;
	const uint pos = index % 128;
	const uint128 bit = uint128(1) << pos;
	_mask[mask] &= ~bit;
	return *this;
}


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
