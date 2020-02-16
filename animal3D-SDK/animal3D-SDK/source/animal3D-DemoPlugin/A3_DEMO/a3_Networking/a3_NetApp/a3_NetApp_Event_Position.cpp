#include "a3_NetApp_Event_Position.h"

Event_Position::Event_Position(float newX, float newY)
{
	x = newX; 
	y = newY;
}

void Event_Position::Dispatch(TextObject* textObj)
{
	textObj->SetPos(x, y);
}
