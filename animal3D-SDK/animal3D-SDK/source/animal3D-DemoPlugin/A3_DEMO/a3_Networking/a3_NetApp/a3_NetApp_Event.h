#ifndef A3_NETAPP_EVENT_H
#define A3_NETAPP_EVENT_H
#include <stdlib.h>
#include "a3_NetApp_TextObject.h"

class Event
{
public:
	// CTOR
	Event() {};
	virtual void Dispatch (TextObject* textObj) = 0;
private:
};

#endif // !A3_NETAPP_EVENT_H

