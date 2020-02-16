#ifndef A3_NETAPP_EVENT_H
#define A3_NETAPP_EVENT_H
#include <stdlib.h>
#include <string>

class Event
{
public:
	// CTOR
	Event() {};
	virtual void Dispatch () = 0;
private:
};

#endif // !A3_NETAPP_EVENT_H

