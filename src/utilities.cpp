#include "utilities.h"

wostream& operator<<(wostream& out, uint128 v)
{
	uint64 high = v >> 64;
	uint64 low = v;
	out << L"0x";
	out << hex << setw(16) << setfill(L'0') << high;
	out << hex << setw(16) << setfill(L'0') << low;
	return out;
}
