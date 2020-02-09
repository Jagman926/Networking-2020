#include "a3_NetApp_Client.h"
#include <string>
#include <algorithm>

void ClientChat::In(char in[512])
{
	// add message to chat buffer
	std::strncpy(buffer[bufferWriteLoc], in, 512);
	// increment current chat buffer
	bufferWriteLoc++;
	// ajust chat view location
	bufferViewOffset = std::max(0, bufferWriteLoc - CHAT_VIEW_MAX);
}

void ClientChat::ClearChatBuffer() 
{ 
	memset(buffer, 0, sizeof buffer); 
	bufferWriteLoc = 0; 
	bufferViewOffset = 0; 
}

//

void ClientInput::In(int asciiIn)
{
	// Add ascii valie to buffer write location
	buffer[bufferWriteLoc] = asciiIn;
	// Increment write location
	bufferWriteLoc++;
}

void ClientInput::ClearLastValue()
{
	// Decrement write location
	bufferWriteLoc--;
	// Set value to null
	buffer[bufferWriteLoc] = 0;
}

void ClientInput::ClearChatBuffer() 
{ 
	memset(buffer, 0, sizeof buffer); 
	bufferWriteLoc = 0; 
}

/* ------------------------------------------------------------- */

