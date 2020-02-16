#include "a3_NetApp_EventManager.h"

void EventManager::AddNewEvent(Event* theEvent)
{
	// add event to the managed array at our current index
	events[eventIterator] = theEvent;
	// increment the index
	eventIterator++;
}

void EventManager::RemoveEvent(int deletionIndex)
{
	// Simply delete the event at the specified index
	delete events[deletionIndex];

}

void EventManager::ProcessEvents()
{
	// Iterate through list of events
	for (int i = 0; i < eventIterator; i++)
	{
		// Dispatch events in order they were added
		events[i]->Dispatch();

		// Remove the dispatched event upon completion
		RemoveEvent(i);

	}
}
