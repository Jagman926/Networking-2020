#include "a3_NetApp_Client.h"
#include <string>
#include <algorithm>

void OutputToChat(char buffer[512])
{
	// add message to chat buffer
	std::strncpy(chatBuffer[chatLoc], inputBuffer, 512);
	// increment current chat buffer
	chatLoc++;
	// ajust chat view location
	chatOffset = std::max(0, chatLoc - CHAT_VIEW_MAX);
}

