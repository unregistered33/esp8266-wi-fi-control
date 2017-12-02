// CommandParser.h
#pragma once

#ifndef _COMMANDPARSER_h
#define _COMMANDPARSER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#endif

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Protocol.h"

using namespace std;

//#include "Provider.h"

class CommandParser
{
private:
	String sCmd;
	
	String commands[MAX_COMMANDS] = { "mode", "level", "reset", "show" };
	String params[MAX_ARGS] = { "-o", "-i", "-l", "-h", "-p", "-low", "-high", "-out", "-in", "-adc1", "-adc2", "-adc3", "-adc4" };
	String modeParams[2] = { "-o", "-i" };
	String levelParams[2] = { "-l", "-h" };
	String showParams[8] = { "-low", "-high", "-out", "-in", "-adc0", "-adc1", "-adc2", "-adc3" };
	String resetParams[1] = { "-p" };

	std::map <String, String > commandMap{
		{ "mode","-o-i" },
		{ "level","-l-h" },
		{ "reset","-p" },
		{ "show","-low-high-out-in-adc0" }
	};
	vector<Base*> parsedCmd;

public:
	CommandParser();
	~CommandParser();
	void parse(String _sCmd);
	void clearClientCommands();
	vector<Base*> getClientCommands();
	//void setProvider(Provider &provider);
private:
//	Provider * provider;
	String strCmd;
	String strParam;
	String strArgs;
	bool isCmdDetected;
	bool isParamDetected;
	bool isArgDetected;
	bool findCmd(String * cmds, size_t szCmds, String cmd);
	void parse2(String str, int i);
	bool cmdValidate(String cmd, String params, String args);
};



