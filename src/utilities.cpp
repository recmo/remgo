#include "utilities.h"
#include <sys/resource.h>

wostream& operator<<(wostream& out, uint128 v)
{
	uint64 high = v >> 64;
	uint64 low = v;
	out << L"0x";
	out << hex << setw(16) << setfill(L'0') << high;
	out << hex << setw(16) << setfill(L'0') << low;
	return out;
}

float cpuTimeUsed()
{
	const int who = RUSAGE_SELF;
	struct rusage usage;
	int ret = getrusage(who, &usage);
	if(ret != 0) {
		return std::nanf("");
	}
	float result = 0.0f;
	result += usage.ru_utime.tv_sec;
	result += usage.ru_utime.tv_usec * 1.0e-6f;
	result += usage.ru_stime.tv_sec;
	result += usage.ru_stime.tv_usec * 1.0e-6f;
	return result;
}

float peakMemoryUsed()
{
	const int who = RUSAGE_SELF;
	struct rusage usage;
	int ret = getrusage(who, &usage);
	if(ret != 0) {
		return std::nanf("");
	}
	float result = 0.0f;
	result += usage.ru_maxrss * 1e-3;
	return result;
}


wstring toUpper(const wstring& string)
{
	wstring result;
	result.reserve(string.size());
	for (wchar_t c: string)
		result.push_back(std::towupper(c));
	return result;
}
