#include <locale>
#include <iostream>
#include "Rotation.h"
#include "BoardPoint.h"
#include "BoardMask.h"
using std::wcerr;
using std::wcout;

#define dumpTypeInfo(Type) \
	do{\
		wcerr << "sizeof("  #Type ") = " << sizeof(Type) << "\t";\
		wcerr << "alignof("  #Type ") = " << alignof(Type) << endl;\
	} while(false)

int main(int argc, char* argv[])
{
	// Set output encodings
	const std::locale mylocale("");
	wcout.sync_with_stdio(false);
	wcout.imbue(mylocale);
	wcerr.sync_with_stdio(false);
	wcerr.imbue(mylocale);
	
	// Print some size info
	dumpTypeInfo(BoardPoint);
	dumpTypeInfo(BoardMask);
	srand(time(0));
	// Rotation::test();
	
	BoardMask mask;
	wcout << mask;
	
	while(1) {
		BoardPoint p;
		wcin >> p;
		if(mask.isSet(p))
			mask.clear(p);
		else
			mask.set(p);
		wcout << mask;
	}
	
	return 0;
}
