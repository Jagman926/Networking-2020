#ifndef A3_NETAPP_EVENT_COLOR
#define A3_NETAPP_EVENT_COLOR

#include "a3_NetApp_Event.h"

class Event_Color : Event
{
public:
	// Default CTOR
	Event_Color() { r = 0, g = 0, b = 0, a = 0; }
	// Data CTOR
	Event_Color(float newR, float newG, float newB, float newA) { r = newR, g = newG, b = newB, a = newA; };
	// ------------------FUNCTIONS -----------------//
	virtual void Dispatch();

private:
	
	// color channels
	float r, g, b, a;

};

#endif // !A3_NETAPP_EVENT_COLOR
