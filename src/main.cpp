#include "utilities.h"
#include <locale>
#include <signal.h>
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
	virtual BoardMask white() override final { return _board.white(); }
	virtual BoardMask black() override final { return _board.black(); }
	virtual void show(wostream& out) override final { out << _board; }
	virtual sint score() override final { return _board.score(); }
	
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
}

BoardPoint BoardEngine::generateMove()
{
	// Collect all moves
	BoardMask moves;
	for(BoardPoint move: _board.validMoves()) {
		// Filter out moves that play own eyes
		bool eye = true;
		if(move.up() && !_board.playerPieces().isSet(move.up()))
			eye = false;
		if(move.down() && !_board.playerPieces().isSet(move.down()))
			eye = false;
		if(move.left() && !_board.playerPieces().isSet(move.left()))
			eye = false;
		if(move.right() && !_board.playerPieces().isSet(move.right()))
			eye = false;
		if(!eye)
			moves.set(move);
	}
	
	// Play a random move
	BoardPoint move = moves.randomPoint();
	_board.play(move);
	return move;
}

GoTextProtocol* gtp = nullptr;

void sighup(int)
{
	wcerr << "SIGHUP received, stopping after next game" << endl;
	if(gtp != nullptr)
		gtp->stop();
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
	// Rotation::test();
	
	BoardEngine engine;
	GoTextProtocol gtp(&engine);
	signal(SIGHUP, &sighup);
	::gtp = &gtp;
	gtp.run();
	
	return 0;
}
