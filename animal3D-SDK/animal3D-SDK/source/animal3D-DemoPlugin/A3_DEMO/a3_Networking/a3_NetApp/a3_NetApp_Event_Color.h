#ifndef A3_NETAPP_EVENT_COLOR
#define A3_NETAPP_EVENT_COLOR

#include "a3_NetApp_Event.h"

class Event_Color : public Event
{
public:
	// Default CTOR
	Event_Color() { r = 0, g = 0, b = 0; };
	// Data CTOR
	Event_Color(float newR, float newG, float newB);
	// ------------------FUNCTIONS -----------------//
	virtual void Dispatch(TextObject* textObj);

private:
	
	// color channels
	float r, g, b;

};

#endif // !A3_NETAPP_EVENT_COLOR
