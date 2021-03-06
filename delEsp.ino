
#include "Protocol.h"
#include "Provider.h"
#include <map>
#include <functional>  
#include <utility> 
#include <vector>
#include <list>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "CommandParser.h"
#include "ESP8266Bridge.h"

using namespace std;

#define MAX_SRV_CLIENTS 5
//default pins state
#define PIN_GPIO_COUNT 8
#define PIN_ADC_COUNT 1

//esp8266 pin map
int pinsGPIO[] = { 0,2,4,5,12,13,14,15,16 }; //  PIN_COUNT 2-pin - small blue LED (LOW - ON| HIGH - OFF)
int pinsADC[] = { 0 };
int modes[] = { OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT }; // PIN_GPIO_COUNT
int levels[] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}; //  PIN_GPIO_COUNT

byte mac[] = {
	0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];


Provider provider(pinsGPIO, PIN_GPIO_COUNT, modes, levels, pinsADC, PIN_ADC_COUNT);//init Provider ;
CommandParser parser;

void processCommand(String command) 
{
	parser.parse(command);// "mode -o 1/2/32 - i 23/4/5 -o 54/41 level -h 2/3/4 -l 12/13/14");//* reset -p qwerty show -l 123 -h 45 -i 67");
	vector<Base *> cmds = parser.getClientCommands();

	for (int i = 0; i < cmds.size(); i++)
	{
		Base * b = cmds.at(i);
		String c = b->getCommandName();
		if (c == "mode")
		{
			MODE * m = static_cast<MODE*>(b);
			provider.RiseThisSHIT(c, m->pin, m->mode);
		}
		if (c == "level")
		{
			LEVEL * l = static_cast<LEVEL*>(b);
			provider.RiseThisSHIT(c, l->pin, l->level);
		}

		if (c == "pwm")
		{
			PWM * pwm = static_cast<PWM*>(b);
			provider.RiseThisSHIT(c, pwm->pin, pwm->val);
		}
	}
	parser.clearClientCommands();
}

void addClient(WiFiClient client)
{
	for (int i = 0; i < MAX_SRV_CLIENTS; i++)
	{
		if (!serverClients[i] || !serverClients[i].connected())
		{
			serverClients[i] = client;
			Serial.println("Client added");
			return;
		}
	}
	//������� ����� � ���������;
	//client.write...
	//client.stop();
}

void setup()
{
	Serial.begin(115200);
	
	delay(500);
	
	WiFi.mode(WIFI_AP);
	Serial.println();
	Serial.print("Setting soft-AP configuration ... ");
	Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

	Serial.print("Setting soft-AP ... ");
	Serial.println(WiFi.softAP("ZAZ", "123456789") ? "Ready" : "Failed!");

	Serial.print("Soft-AP IP address = ");
	Serial.println(WiFi.softAPIP());

	server.begin();
	server.setNoDelay(true);
}

void loop()
{
	WiFiClient client = server.available();
	if (client) {
		Serial.println("Client connected");
		if (client.connected())
			addClient(client);
	}

	for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
		if (serverClients[i] && serverClients[i].connected())
		{
			String clientCommand = "";

			int c = -1;
			int bytes_read = 0;
			c = serverClients[i].read();
			while (c != -1)
			{
				bytes_read++;
				clientCommand += char(c);

				c = serverClients[i].read();
			}

			if (bytes_read > 0)
			{
				Serial.println(clientCommand);
				for (int j = 0; j < MAX_SRV_CLIENTS; j++)
					if (serverClients[j] && serverClients[j].connected())
						serverClients[j].write(clientCommand.c_str(), bytes_read);
				processCommand(clientCommand);
				clientCommand = "";
			}
			serverClients[i].flush();
		}
	}
	//Serial debug
	if (Serial.available() > 0)
	{
		String debugStr = Serial.readString();
		if (debugStr != "")
			processCommand(debugStr);
		debugStr = "";
	}

}

