#ifndef A3_NETAPP_EVENTMANAGER_H
#define A3_NETAPP_EVENTMANAGER_H
#include <stdlib.h>
#include "a3_NetApp_Event.h"

class EventManager
{
public:

	// Default constructor
	EventManager();

	// managed array of events
	Event* events[10];
	// iterator for reading through events
	int eventIterator = 0;

	// the text object we're modifying
	TextObject textObject;

	// ------------------FUNCTIONS -----------------//
	
	// Adds a new event to the list of events
	void AddNewEvent(Event* theEvent);
	// Removes an event at the specified index
	void RemoveEvent(int deletionIndex);
	// Process events in the lists and calls their dispatch function
	void ProcessEvents();

};

#endif // !A3_NETAPP_EVENTMANAGER_H
