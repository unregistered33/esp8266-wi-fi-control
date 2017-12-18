// Provider.h

#pragma once

#ifndef _PROVIDER_h
#define _PROVIDER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif

#include <vector>
#include <functional>
#include "ESP8266Bridge.h"
#include "CommandParser.h"

//GLOBAL SECTION
enum class EventCommands { MODE, LEVEL, RESET, SHOW };
typedef EventCommands TT;

using namespace std;
class Provider
{
private:
	struct PIN
	{
		int addr;
		int pMode;
		int level;
		bool pwm;
	};

	struct PIN_ADC
	{
		int addr;
		int level;
	};
	//пока что удалил
	//CommandParser parser;

	vector<PIN> pinGPIOCfg;
	vector<PIN_ADC> pinADCcfg;

	int findPinGPIO(int addr);
	PIN_ADC* findPinADC(int addr);

	ESP8266Bridge<String, int, int> esp;

public:
	Provider() = default;

	Provider(int * pinAddrs, int cntPins);

	Provider(int * pinGPIOAddrs, int cntGPIOPins, int * pinGPIOModes, int * pinGPIOLevels, int * pinADCAddrs, int cntADCPins);

	// 1 - syntax error _mode, 2 - pin not found, 0 - OK
	int setPinMode(int _addr, int _mode);
	
	// 1 - syntax error _level, 2 - wrong mode, 3 - pin not found, 0 - OK
	int setPinLevel(int _addr, int _level);
	int setPinPWM(int _addr, int _level);
	int GetPinMode(int addr);
	int GetPinLevel(int addr);
	int GetPinADC(int addr);

	void OnMode(int _addr, int _mode);
	void OnLevel(int _addr, int _level);
	void OnShow(int _prop, int pin_addr);
	void OnPWM(int _addr, int _level);
	void OnReset();
	void OnADC(int _addr);
	void RiseThisSHIT(String cm, int a1, int a2);
	void ProcessCommandLine(String cmdl);
	
};