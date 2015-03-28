#include "GoTextProtocol.h"

#define registerCommand(command)\
	_commands[L"" #command] = &GoTextProtocol::command;

#define numArguments(N) \
	if(_arguments.size() != N) { \
		writeError(L"syntax error"); \
		return; \
	}

GoTextProtocol::GoTextProtocol(Engine* engine, wistream& in, wostream& out)
: _engine(engine)
, _in(in)
, _out(out)
, _quit(false)
, _commands()
, _id()
, _command()
, _arguments()
{
	assert(_engine != nullptr);
	
	// Administrative commands (required)
	registerCommand(protocol_version);
	registerCommand(name);
	registerCommand(version);
	registerCommand(known_command);
	registerCommand(list_commands);
	registerCommand(quit);
	
	// Setup commands (required)
	registerCommand(boardsize);
	registerCommand(clear_board);
	registerCommand(komi);
	
	// Core play commands (required)
	registerCommand(play);
	registerCommand(genmove);
}

void GoTextProtocol::run()
{
	while(!_quit && _in.good()) {
		if(!readCommand())
			continue;
		
		wcerr << _id << ": " << _command << " " << _arguments << endl;
		
		// Find command
		auto it = _commands.find(_command);
		if(it != _commands.end())
			(this->*it->second)();
		else
			writeError(L"unknown command");
	}
}

void GoTextProtocol::protocol_version()
{
	numArguments(0);
	writeResponse(L"2");
}

void GoTextProtocol::name()
{
	numArguments(0);
	writeResponse(L"RemGo");
}

void GoTextProtocol::version()
{
	numArguments(0);
	writeResponse(L"0.0.1");
}

void GoTextProtocol::known_command()
{
	numArguments(1);
	auto it = _commands.find(_arguments[0]);
	if(it != _commands.end())
		writeResponse(L"true");
	else
		writeResponse(L"false");
}

void GoTextProtocol::list_commands()
{
	numArguments(0);
	wstring response;
	for(auto it: _commands) {
		if(!response.empty())
			response.append(L"\n");
		response.append(it.first);
	}
	writeResponse(response);
}

void GoTextProtocol::quit()
{
	numArguments(0);
	writeResponse();
	_quit = true;
}

void GoTextProtocol::boardsize()
{
	numArguments(1);
	if(_arguments[0] != L"19") {
		writeError(L"unacceptable size");
		return;
	}
	writeResponse();
}

void GoTextProtocol::clear_board()
{
	numArguments(0);
	_engine->reset();
	writeResponse();
}

void GoTextProtocol::komi()
{
	numArguments(1);
	writeResponse();
}

void GoTextProtocol::play()
{
	numArguments(2);
	
	wistringstream in(_arguments[1]);
	BoardPoint move;
	in >> move;
	_engine->receiveMove(move);
	
	writeResponse();
	// writeError(L"illegal move");
}

void GoTextProtocol::genmove()
{
	numArguments(1);
	const BoardPoint move = _engine->generateMove();
	if(!move.isValid()) {
		writeResponse(L"pass");
		// writeResponse(L"resign");
	} else {
		wcerr << move << endl;
		wostringstream out;
		out << move;
		writeResponse(out.str());
	}
}


bool GoTextProtocol::readCommand()
{
	// Clear
	_id.clear();
	_command.clear();
	_arguments.clear();
	
	// Commands are a line of tokens separated by spaces
	wstring line;
	getline(_in, line);
	wistringstream iss(line);
	vector<wstring> parts;
	while(iss.good()) {
		wstring part;
		iss >> part;
		if(!part.empty())
			parts.push_back(part);
	}
	
	// Empty lines are ignored
	if(parts.empty())
		return false;
	
	// Comments start with a #
	if(parts[0][0] == L'#')
		return false;
	
	// The first field is an optional id
	if(isNumber(parts[0])) {
		assert(parts.size() >= 2);
		_id = parts[0];
		_command = parts[1];
		parts.erase(parts.begin(), parts.begin() + 2);
		_arguments = parts;
	} else {
		assert(parts.size() >= 1);
		_id.clear();
		_command = parts[0];
		parts.erase(parts.begin(), parts.begin() + 1);
		_arguments = parts;
	}
	return true;
}

void GoTextProtocol::writeResponse(const wstring& result)
{
	_out << "=" << _id << " " << result << endl << endl;
}

void GoTextProtocol::writeError(const wstring& message)
{
	_out << "?" << _id << " " << message << endl << endl;
}

void GoTextProtocol::writeComment(const wstring& message)
{
	wstring line;
	wistringstream iss(message);
	while(iss.good()) {
		getline(iss, line);
		_out << "# " << line << endl;
	}
}

bool GoTextProtocol::isNumber(const wstring& string)
{
	if(string.empty())
		return false;
	for(wchar_t c: string)
		if(c < L'0' || c > L'9')
			return false;
	return true;
}
