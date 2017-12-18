// 
// 
// 
#include <vector>
#include "CommandParser.h"
#include "Protocol.h"


CommandParser::CommandParser() //:commands{ "mode", "level", "reset", "show" },params{ "-o", "-i", "-l", "-h", "-p" }
{
	//commands[4] = { "mode", "level", "reset", "show" };
	isCmdDetected = false;
	isParamDetected = false;
	isArgDetected = false;
	//parse2(string(sCmd.c_str()), 0);
}

CommandParser::~CommandParser()
{
	//delete provider;
}

bool CommandParser::findCmd(String * cmds, size_t szCmds, String cmd)
{
	for (int i = 0; i < szCmds; i++)
		if (cmd.indexOf(cmds[i]) != -1)
			return 1;

	return 0;
}

bool CommandParser::cmdValidate(String cmd, String params, String args)
{
	if (commandMap.at(cmd).indexOf(params) != -1)
	{
		return true;
	}
	return false;
}


void CommandParser::parse2(String str, int i)
{
	i++;
	String command = str.substring(0, str.indexOf(" ", 1));
	str = str.substring(str.indexOf(" ", 1) + 1, str.length());

	if (findCmd(commands, MAX_COMMANDS, String(command.c_str()))) //������� �������
	{
		if (!isCmdDetected) //���� �� ������� ������� �� �����? ���� ���, ������� ��� ������ �������
		{
			strCmd = command;
			isCmdDetected = true;
		}	
		else //���� ��� ���� ������� ����� ������� �� ������ � ������ �� �� ����� ���������
		{
			if (strCmd != command)
				strCmd = command;
		}
			
		//i = 0;
		
	}
	else if (command.indexOf("-") != -1)/*(findCmd(params, MAX_ARGS, command))*///������ ��������
	{
		if (isCmdDetected)// ���� ���� �������� �������
		{
			if (!isParamDetected) //���� �������� ��� �� ��� ������
			{
				strParam = command;
				isParamDetected = true;
			}
			else// ���� �������� ��� ��� ������, �� �� ���������� � ��������� �������
			{
				strCmd += command;
				//isParamDetected = false;
			}
		}
		else //���� �������� ������, �� ������� �� ���� �� ����
		{
			isParamDetected = false;
		}

		i = 1;
	}
	else if (String(command.c_str()) != "")//���� ���-�� �������, �� �� ������� � �� ��������, ������������ ��� ��� �����������
	{
		if (isCmdDetected && isParamDetected) //���� ������� � �������� ���� ������� �����
		{
			strArgs = command;

			isParamDetected = false;
			isArgDetected = false;

			String COMMAND = strCmd + " " + strParam + " " + strArgs;

			//���� ������������� �� ���������� ���� $
			//Serial.println(COMMAND);

			String pin;

			if (strCmd == "mode")
			{
				while (strArgs.length() > 0)
				{
					int ind = strArgs.indexOf("/");
					if (ind == -1)
					{
						ind = strArgs.length();
					}

					pin = strArgs.substring(0, ind);
					strArgs.remove(0, ind + 1);

					MODE m;
					m.pin = pin.toInt();
					if (strParam == "-o")
						m.mode = OUTPUT;
					else if (strParam == "-i")
						m.mode = INPUT;
					else
						m.mode = -1;//unknown
					parsedCmd.push_back(new MODE(m));
				}
			}
			else if (strCmd == "level")
			{
				while (strArgs.length() > 0)
				{
					int ind = strArgs.indexOf("/");
					if (ind == -1)
					{
						ind = strArgs.length();
					}

					pin = strArgs.substring(0, ind);
					strArgs.remove(0, ind + 1);

					LEVEL l;
					l.pin = pin.toInt();
					if (strParam == "-h")
						l.level = HIGH;
					else if (strParam == "-l")
						l.level = LOW;
					else
						l.level = -1;//unknown

					parsedCmd.push_back(new LEVEL(l));
				}
			}
			else if (strCmd == "pwm")
			{
				while (strArgs.length() > 0)
				{
					int ind = strArgs.indexOf("/");
					if (ind == -1)
					{
						ind = strArgs.length();
					}

					pin = strArgs.substring(0, ind);
					strArgs.remove(0, ind + 1);

					PWM pwm;
					pwm.pin = pin.toInt();
					strParam = strParam.substring(1, strParam.length());
					pwm.val = strParam.toInt();
					parsedCmd.push_back(new PWM(pwm));
				}
			}
		}
		
		strArgs = "";
		strParam = "";
	}

	if (str.indexOf(" ", 1) == -1)
	{
		return;
	}
	
	if (str.length() > 0)
	{
		parse2(str, i);
		return;
	}
}

vector<Base*> CommandParser::getClientCommands() 
{
	return parsedCmd;
}

void CommandParser::clearClientCommands()
{
	parsedCmd.clear();
	parsedCmd.shrink_to_fit();
}

void CommandParser::parse(String _sCmd)
{
	sCmd = _sCmd +" ";
	parse2(sCmd, 0);
}
/*
void CommandParser::setProvider(Provider &_provider)
{
	provider = &_provider;
}*/