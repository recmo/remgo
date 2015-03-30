#pragma once
#include "utilities.h"
#include "BoardPoint.h"
#include "BoardMask.h"

class GoTextProtocol {
public:
	class Engine;
	GoTextProtocol(Engine* engine, wistream& in = wcin, wostream& out = wcout);
	~GoTextProtocol() { }
	
	void run();
	
	void stop();
	
private:
	typedef void (GoTextProtocol::*Command)();
	Engine* const _engine;
	wistream& _in;
	wostream& _out;
	bool _quit;
	map<wstring, Command> _commands;
	
	// Input command
	wstring _id;
	wstring _command;
	vector<wstring> _arguments;
	
	//
	bool _lastGenmove;
	float _komi;
	bool _stopping;
	bool _gameStarted;
	
	// Commands
	void protocol_version();
	void name();
	void version();
	void known_command();
	void list_commands();
	void quit();
	void boardsize();
	void clear_board();
	void komi();
	void play();
	void genmove();
	
	void showboard();
	void list_stones();
	void final_score();
	void get_random_seed();
	void cputime();
	void memory();
	
	void game_over();
	
	// Readers/writers
	bool readCommand();
	void writeResponse(const wstring& result = L"");
	void writeError(const wstring& message);
	void writeComment(const wstring& message);
	static bool isNumber(const wstring& string);
};

class GoTextProtocol::Engine {
public:
	virtual void reset() = 0;
	virtual void receiveMove(BoardPoint move) = 0;
	virtual BoardPoint generateMove() = 0;
	virtual BoardMask white() = 0;
	virtual BoardMask black() = 0;
	virtual void show(wostream& out) = 0;
	virtual sint score() = 0;
};
