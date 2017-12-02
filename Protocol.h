// Protocol.h
#pragma once
#ifndef _PROTOCOL_h
#define _PROTOCOL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif

//using namespace std;

#define MAX_COMMANDS 4
#define MAX_ARGS 13



struct Base
{
public:
	virtual String getCommandName()=0;
	virtual ~Base() {};// { cout << "Destr base" << endl; };
};


struct MODE :public Base
{
public:
	int mode;
	int pin;
	String getCommandName() override
	{
		return String("mode");
	}
	MODE() = default;
	MODE(const MODE &mode)
	{
		this->mode = mode.mode;
		this->pin = mode.pin;
	}

	~MODE()
	{
		
	}

};

struct LEVEL :public Base
{
	int level;
	int pin;
	String getCommandName() override
	{
		return String("level");
	}
	LEVEL() = default;
	LEVEL(const LEVEL &level)
	{
		this->level = level.level;
		this->pin = level.pin;
	}

	~LEVEL()
	{

	}
};

struct PWM :public Base
{
	int level;
	int pin;
	String getCommandName() override
	{
		return String("level");
	}
	PWM() = default;
	PWM(const PWM &pwm)
	{
		this->level = pwm.level;
		this->pin = pwm.pin;
	}

	~PWM()
	{

	}
};

struct SHOW :public Base 
{
	String param;
	String getCommandName() override
	{
		return String("show");
	}
};

struct RESET :public Base
{
	String pwd;
	String getCommandName() override
	{
		return String("reset");
	}
};




