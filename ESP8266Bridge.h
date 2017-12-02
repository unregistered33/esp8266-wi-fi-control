// ESP8266Bridge.h
#pragma once
#ifndef _ESP8266BRIDGE_h
#define _ESP8266BRIDGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif
#include <map>
#include <functional>  // for std::function
#include <utility>     // for std::forward and std::move
#include <vector>
#include <list>

using namespace std;


template<class EventType, class... HandlerArgs>
class ESP8266Bridge
{
public:
	using EventHandler = std::function< void(HandlerArgs...) >;
	//ESP8266Bridge() = default;

	void register_event(const EventType event, EventHandler handler)
	{
		_handlers[std::move(event)].push_back(std::forward<EventHandler>(handler));
	}

	void raise_event(const EventType event, HandlerArgs... args)
	{
		for (const auto& handler : ESP8266Bridge::_handlers.at(event)) {
			handler(std::forward<HandlerArgs>(args)...);
		}
	}

	// disallow copying and assigning
	/*Subject(const Subject&) = delete;
	Subject& operator=(const Subject&) = delete;
	*/
private:
	std::map<EventType, std::vector<EventHandler>> _handlers;
};


