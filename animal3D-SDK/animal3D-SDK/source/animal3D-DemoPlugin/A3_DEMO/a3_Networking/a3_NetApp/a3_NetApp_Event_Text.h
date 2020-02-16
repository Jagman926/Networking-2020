#ifndef A3_NETAPP_EVENT_TEXT
#define A3_NETAPP_EVENT_TEXT

#include "a3_NetApp_Event.h"

class Event_Text : public Event
{
public:
	

	// Data CTOR
	Event_Text(char newText[512]);

	// ------------------FUNCTIONS -----------------//
	virtual void Dispatch(TextObject* textObj);
private:
	// string of text
	char text[512];
};

#endif // !A3_NETAPP_EVENT_TEXT

