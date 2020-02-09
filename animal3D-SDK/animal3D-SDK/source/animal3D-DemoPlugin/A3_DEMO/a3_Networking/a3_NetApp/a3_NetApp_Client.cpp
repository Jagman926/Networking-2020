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

void ClientChat::ClearLastChatMessage()
{
	if (bufferWriteLoc != 0)
	{
		// Decrement write location
		bufferWriteLoc--;
		// Clear buffer
		memset(buffer[bufferWriteLoc], 0, sizeof buffer[bufferWriteLoc]);
	}
}

void ClientChat::ClearChatBuffer() 
{ 
	// Clear buffer
	memset(buffer, 0, sizeof buffer); 
	// Reset buffer write to start
	bufferWriteLoc = 0; 
	// Reset buffer view offset to start
	bufferViewOffset = 0; 
}

// -------------------------------------------------------------------------------

void ClientInput::In(int asciiIn)
{
	// Add ascii valie to buffer write location
	buffer[bufferWriteLoc] = asciiIn;
	// Increment write location
	bufferWriteLoc++;
}

void ClientInput::ClearLastValue()
{
	if (bufferWriteLoc != 0)
	{
		// Decrement write location
		bufferWriteLoc--;
		// Set value to null
		buffer[bufferWriteLoc] = 0;
	}
}

void ClientInput::ClearChatBuffer() 
{ 
	// Copy current input buffer to last buffer
	strcpy(lastInputBuffer, buffer);
	// Clear current input buffer
	memset(buffer, 0, sizeof buffer); 
	// Reset buffer write to start
	bufferWriteLoc = 0; 
}

void ClientInput::ClearLastBuffer()
{
	// Clear lastInputBuffer
	memset(lastInputBuffer, 0, sizeof lastInputBuffer);
}

/* ------------------------------------------------------------- */

