#ifndef A3_NETAPP_EVENT_TEXT
#define A3_NETAPP_EVENT_TEXT

#include "a3_NetApp_Event.h"

class Event_Text : Event
{
public:
	// Default CTOR
	Event_Text() { };

	// Data CTOR
	Event_Text(char newText[512]) { strcpy(text, newText); }

	// ------------------FUNCTIONS -----------------//
	virtual void Dispatch();
private:
	// string of text
	char text[512] = { 0 };
};

#endif // !A3_NETAPP_EVENT_TEXT

