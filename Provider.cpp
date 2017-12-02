
#include "Provider.h"


int Provider::findPinGPIO(int addr)
{
	for (int i = 0; i < pinGPIOCfg.size(); i++)
	{
		PIN pin = pinGPIOCfg[i];
		if (pin.addr == addr)
			return i;
	}
	return -1;
}

Provider::PIN_ADC* Provider::findPinADC(int addr)
{
	for (int i = 0; i < pinADCcfg.size(); i++)
	{
		PIN_ADC pin = pinADCcfg[i];
		if (pin.addr == addr)
			return &pin;
	}
	return NULL;
}

Provider::Provider(int * pinAddrs, int cntPins)
	{
		for (int i = 0; i < cntPins; i++)
		{
			PIN tmpPin;
			tmpPin.addr = pinAddrs[i];
			tmpPin.pMode = OUTPUT;
			tmpPin.level = LOW;
			pinGPIOCfg.push_back(tmpPin);
			//pinMode(pinAddrs[i], OUTPUT);
			//digitalWrite(pinAddrs[i],LOW);
		}
	}

Provider::Provider(int * pinGPIOAddrs, int cntGPIOPins, int * pinGPIOModes, int * pinGPIOLevels, int * pinADCAddrs, int cntADCPins)
	{
		for (int i = 0; i < cntGPIOPins; i++)
		{
			struct PIN tmpPin;
			tmpPin.addr = pinGPIOAddrs[i];
			tmpPin.pMode = pinGPIOModes[i];
			tmpPin.level = pinGPIOLevels[i];
			pinGPIOCfg.push_back(tmpPin);
			pinMode(pinGPIOAddrs[i], pinGPIOModes[i]);
			digitalWrite(pinGPIOAddrs[i], pinGPIOLevels[i]);
		}

		for (int i = 0; i < cntADCPins; i++)
		{
			struct PIN_ADC pin;
			pin.level = digitalRead(pinADCAddrs[i]);
			pinADCcfg.push_back(pin);
		}

		esp.register_event("mode", [&](int _addr, int _mode)
		{
			OnMode(_addr, _mode);
		});

		esp.register_event("level", [&](int _addr, int _mode)
		{
			OnLevel(_addr, _mode);
		});

		esp.register_event("pwm", [&](int _addr, int _mode)
		{
			OnPWM(_addr, _mode);
		});

		esp.register_event("reset", [&](int _notUse, int _notUse1)
		{
			OnReset();
		});

		esp.register_event("adc", [&](int _addr, int _notUse)
		{
			OnADC(_addr);
		});
	}

int Provider::setPinMode(int _addr, int _mode) // 1 - syntax error _mode, 2 - pin not found, 0 - OK
{
	if (_mode != INPUT && _mode != OUTPUT)
		return 1;
	int pinId = findPinGPIO(_addr);
	if (pinId != -1)
	{
		pinGPIOCfg[pinId].pMode = _mode;
		pinGPIOCfg[pinId].pwm = false;
		pinGPIOCfg[pinId].level = LOW;
		return 0;
	}
		
	else
		return 2;
}


int Provider::setPinLevel(int _addr, int _level)// 1 - syntax error _level, 2 - pin not found, 3 - mode must be OUTPUT, 0 - OK
{
	if (_level != HIGH && _level != LOW)
		return 1;

	int pinId = findPinGPIO(_addr);
	if (pinId != -1)
	{
		if (pinGPIOCfg[pinId].pMode == INPUT)
			return 3;
		pinGPIOCfg[pinId].level = _level;
		pinGPIOCfg[pinId].pwm = false;
		return 0;
	}	
	else
		return 2;
}

int Provider::setPinPWM(int _addr, int _level) // 1 - wrong level, 2 - pin not found, 3 - mode must be OUTPUT, 0 - OK
{
	if (_level < 0 || _level > 1023)
		return 1;
	int pinId = findPinGPIO(_addr);
	if (pinId != -1)
	{
		if (pinGPIOCfg[pinId].pMode == INPUT)
			return 3;
		pinGPIOCfg[pinId].level = _level;
		pinGPIOCfg[pinId].pwm = true;
		return 0;
	}
	else
		return 2;
}

int Provider::GetPinADC(int addr)//0-возвращаем значение, 1 - нет такого пина
{
	PIN_ADC * pin = findPinADC(addr);
	if (pin)
	{
		return pin->level;
	}
	return 1;
}

int Provider::GetPinMode(int _addr)
{
	PIN pin;
	int pinId = findPinGPIO(_addr);
	if (pinId != -1)
	{
		pin = pinGPIOCfg[pinId];
		return pin.pMode;
	}
	else
		return -1;
}

int Provider::GetPinLevel(int _addr)
{
	PIN pin;
	int pinId = findPinGPIO(_addr);
	if (pinId != -1)
	{
		pin = pinGPIOCfg[pinId];
		return pin.level;
	}
	else
		return -1;
}

void Provider::OnMode(int _addr, int _mode)
{
	int res = setPinMode(_addr, _mode);
	if (res == 0)
		pinMode(_addr, _mode);
	else if (res == 1)
	{
		Serial.print("No such MODE: ");
		Serial.print(_mode);
		Serial.print(". Ex(");
		Serial.print(res);
		Serial.println(")");
	}
	else
	{
		Serial.print("Pin: ");
		Serial.print(_addr);
		Serial.print(" not found");
	}
}

void Provider::OnLevel(int _addr, int _level)// 1 - syntax error _level, 2 - pin not found, 3 - mode must be OUTPUT, 0 - OK
{
	int res = setPinLevel(_addr, _level);
	
	if (res == 0)
		digitalWrite(_addr, _level);
	else if (res == 1)
	{
		Serial.print("No such LEVEL: ");
		Serial.print(_level);
		Serial.print(". Ex(");
		Serial.print(res);
		Serial.println(")");
	}
	else if (res == 2)
	{
		Serial.print("Pin: ");
		Serial.print(_addr);
		Serial.print(" not found. Ex(");
		Serial.print(res);
		Serial.println(")");
	}
	else if (res == 3)
	{
		Serial.print("Error pin(");
		Serial.print(_addr);
		Serial.print("). MODE must be OUTPUT. Ex(");
		Serial.print(res);
		Serial.println(")");
	}
}

void Provider::OnShow(int _prop, int pin_addr)
{

}

void Provider::OnPWM(int _addr, int _level)// 1 - wrong level, 2 - mode must be OUTPUT, 3 - pin not found, 0 - OK
{
	int res = setPinPWM(_addr, _level);
	if (res == 0)
		analogWrite(_addr, _level);
	else if (res == 1)
	{
		Serial.print("LEVEL out of range (0-1023): ");
		Serial.print(_level);
		Serial.print(". Ex(");
		Serial.print(res);
		Serial.println(")");
	}
	else if (res == 2)
	{
		Serial.print("Pin: ");
		Serial.print(_addr);
		Serial.print(" not found. Ex(");
		Serial.print(res);
		Serial.println(")");
	}
	else if (res == 3)
	{
		Serial.print("Error pin(");
		Serial.print(_addr);
		Serial.print("). MODE must be OUTPUT. Ex(");
		Serial.print(res);
		Serial.println(")");
	}
}

void Provider::OnADC(int _addr)//записывае значение в карту; 1 - не найден пин
{
	PIN_ADC * pin = findPinADC(_addr);
	if (pin)
	{
		pin->level = analogRead(_addr);
	}
	else
		Serial.println(1);
}

void Provider::OnReset()
{
	Serial.println("onReset:");
	Serial.println();
}

void Provider::RiseThisSHIT(String cm, int a1, int a2)
{
	esp.raise_event(cm, a1, a2);
}


void Provider::ProcessCommandLine(String cmdl)
{

}


