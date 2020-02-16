#include "a3_NetApp_Event_Color.h"

Event_Color::Event_Color(float newR, float newG, float newB)
{
	r = newR;
	g = newG; 
	b = newB;
}

void Event_Color::Dispatch(TextObject* textObj)
{
	textObj->SetColor(r, g, b);
}
