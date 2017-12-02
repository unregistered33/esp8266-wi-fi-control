
#include "Protocol.h"
#include "Provider.h"
#include <map>
#include <functional>  
#include <utility>     // for std::forward and std::move
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
	}
	parser.clearClientCommands();
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
	uint8_t i;

	if (server.hasClient()) {
		for (i = 0; i < MAX_SRV_CLIENTS; i++) {
			if (!serverClients[i] || !serverClients[i].connected()) {
				if (serverClients[i]) serverClients[i].stop();
				serverClients[i] = server.available();
				continue;
			}
		}
		WiFiClient serverClient = server.available();
		serverClient.stop();
	}

	for (i = 0; i < MAX_SRV_CLIENTS; i++) {
		if (serverClients[i] && serverClients[i].connected()) {
			int len = serverClients[i].available();
			String clientCommand;
			if (serverClients[i].available()) 
			{
				while (len != 0)
				{
					int len = serverClients[i].available();
					if (len > 1460)
						len = 1460;
					uint8_t sbuf[len];
					serverClients[i].readBytes(sbuf, len);

					for (int j = 0; j < MAX_SRV_CLIENTS; j++) 
					{
						if (serverClients[j] && serverClients[j].connected()) 
						{
							serverClients[j].write(sbuf, len);
							//delay(1);
						}
					}
					clientCommand = (char*)sbuf;
					if (clientCommand != "") 
					{
						processCommand(clientCommand);
						clientCommand = "";
					}
				}
			}
		}
	}
}

