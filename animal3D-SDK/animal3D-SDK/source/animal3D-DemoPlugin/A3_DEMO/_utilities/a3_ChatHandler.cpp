#include "a3_ChatHandler.h"
#include <string>
#include <algorithm>

void ChatIn(a3_ChatHandler* chat, char in[512])
{
	// add message to chat buffer
	std::strncpy(chat->buffer[chat->bufferWriteLoc], in, 512);
	// increment current chat buffer
	chat->bufferWriteLoc++;
	// ajust chat view location
	chat->bufferViewOffset = std::max(0, chat->bufferWriteLoc - chat->chatViewMax);
}

void ClearLastChatMessage(a3_ChatHandler* chat)
{
	if (chat->bufferWriteLoc != 0)
	{
		// Decrement write location
		chat->bufferWriteLoc--;
		// Clear buffer
		memset(chat->buffer[chat->bufferWriteLoc], 0, sizeof chat->buffer[chat->bufferWriteLoc]);
	}
}

void ClearChatBuffer(a3_ChatHandler* chat)
{
	// Clear buffer
	memset(chat->buffer, 0, sizeof chat->buffer);
	// Reset buffer write to start
	chat->bufferWriteLoc = 0;
	// Reset buffer view offset to start
	chat->bufferViewOffset = 0;
}