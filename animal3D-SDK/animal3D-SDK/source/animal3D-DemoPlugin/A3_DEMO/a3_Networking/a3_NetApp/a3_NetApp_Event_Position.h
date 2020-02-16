#ifndef A3_NETAPP_EVENT_POSITON
#define A3_NETAPP_EVENT_POSITION

#include "a3_NetApp_Event.h"

class Event_Position : Event
{
public:
	// Default CTOR
	Event_Position() { x = 0, y = 0; }
	// Data CTOR
	Event_Position(float newX, float newY) { x = newX, y = newY; };
	// ------------------FUNCTIONS -----------------//
	virtual void Dispatch();

private:

	// coordinates
	float x, y;

};

#endif // !A3_NETAPP_EVENT_POSITON



