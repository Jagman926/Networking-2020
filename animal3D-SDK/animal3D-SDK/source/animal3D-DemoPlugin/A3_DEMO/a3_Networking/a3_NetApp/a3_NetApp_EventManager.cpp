#include "a3_NetApp_EventManager.h"


EventManager::EventManager()
{
	textObject.SetText("Testing!");
	textObject.SetPos(0.7f, 0.7f);
	textObject.SetColor(1, 0, 0, 1);
}

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
	memset(events[deletionIndex], 0, sizeof(events[deletionIndex]));

}

void EventManager::ProcessEvents()
{
	// Iterate through list of events
	for (int i = 0; i < eventIterator; i++)
	{
		if (events[i] != NULL)
		{
			// Dispatch events in order they were added
			events[i]->Dispatch(&textObject);

			// Remove the event when its dispatched
			RemoveEvent(i);
		}
	}
	// Reset iterator
	eventIterator = 0;

}
