#ifndef A3_NETAPP_TEXTOBJECT_H
#define A3_NETAPP_TEXTOBJECT_H
#include <string>

struct TextObject
{
public:
	char textBuffer[512];
	float r, g, b, a;
	float xPos, yPos;

	void SetText(char buffer[512]) { strncpy(textBuffer, buffer, 512); };
	void SetColor(float rVal, float gVal, float bVal, float aVal = 1) { r = rVal; g = gVal; b = bVal; a = aVal; };
	void SetPos(float x, float y) { xPos = x; yPos = y; };
};

#endif // !A3_NETAPP_TEXTOBJECT_H

