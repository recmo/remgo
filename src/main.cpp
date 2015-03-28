#include "utilities.h"
#include <locale>
#include "Rotation.h"
#include "BoardPoint.h"
#include "BoardMask.h"
#include "Board.h"
#include "GoTextProtocol.h"

#define dumpTypeInfo(Type) \
	do{\
		wcerr << "sizeof("  #Type ") = " << sizeof(Type) << "\t";\
		wcerr << "alignof("  #Type ") = " << alignof(Type) << endl;\
	} while(false)


class BoardEngine: public GoTextProtocol::Engine {
public:
	BoardEngine(): _board() { }
	virtual ~BoardEngine() { }
	
	virtual void reset() override final;
	virtual void receiveMove(BoardPoint move) override final;
	virtual BoardPoint generateMove() override final;
	
private:
	Board _board;
};

void BoardEngine::reset()
{
	_board = Board();
}

void BoardEngine::receiveMove(BoardPoint move)
{
	_board.play(move);
	// wcerr << _board << endl;
}

BoardPoint BoardEngine::generateMove()
{
	const BoardPoint move = _board.validMoves().firstPoint();
	_board.play(move);
	// wcerr << _board << endl;
	return move;
}

int main(int argc, char* argv[])
{
	// Set output encodings
	const std::locale mylocale("");
	wcout.sync_with_stdio(false);
	wcerr.sync_with_stdio(false);
	wcin.sync_with_stdio(false);
	wcout.imbue(mylocale);
	wcerr.imbue(mylocale);
	wcin.imbue(mylocale);
	
	// Print some size info
	dumpTypeInfo(BoardPoint);
	dumpTypeInfo(BoardMask);
	srand(time(0));
	// Rotation::test();
	
	BoardEngine engine;
	GoTextProtocol gtp(&engine);
	gtp.run();
	
	return 0;
}
