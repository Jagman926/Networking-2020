#include "a3_NetApp_Event_Text.h"

Event_Text::Event_Text(char newText[512])
{
	strncpy(text, newText, 512);
}

void Event_Text::Dispatch(TextObject* textObj)
{
	textObj->SetText(text);
}
