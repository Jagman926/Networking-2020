#ifndef A3_NETAPP_EVENTMANAGER_H
#define A3_NETAPP_EVENTMANAGER_H
#include <stdlib.h>
#include "a3_NetApp_Event.h"

struct TextObject
{
public:
	char textBuffer[512];
	float r, g, b, a;
	float xPos, yPos;

	void SetText(char buffer[512]) { strncpy(textBuffer, buffer, 512); };
	void SetColor(float rVal, float gVal, float bVal, float aVal = 1) { r = rVal; g = gVal; b = bVal; a = aVal; };
	void SetPos(float x, float y) { xPos = x; yPos = y; };
};

class EventManager
{
public:
	// managed array of events
	Event* events[100] = {};
	// iterator for reading through events
	int eventIterator = 0;

	// ------------------FUNCTIONS -----------------//
	// Adds a new event to the list of events
	void AddNewEvent(Event* theEvent);
	// Removes an event at the specified index
	void RemoveEvent(int deletionIndex);
	// Process events in the lists and calls their dispatch function
	void ProcessEvents();

};

#endif // !A3_NETAPP_EVENTMANAGER_H
