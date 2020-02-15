#ifndef A3_NETAPP_CLIENT_H
#define A3_NETAPP_CLIENT_H

/* --------------- Client Sub Structures ----------------------- */

struct ClientChat
{
public:
	// const variables
	const int CHAT_VIEW_MAX = 22;

	// Chat container
	char buffer[512][512];
	int bufferWriteLoc;
	int bufferViewOffset;
	int hideChat;
	int unreadMessages;

	//constructors
	ClientChat() { bufferWriteLoc = 0, bufferViewOffset = 0; hideChat = 0; unreadMessages = 0; };

	// Functions ------------------------------------------

	// adds buffer to chat buffer at current write location
	// Params
	//	param1 (char[512]): buffer input
	void In(char buffer[512]);
	// clears entire chat buffer
	void ClearChatBuffer();
	// clears last inputted chat buffer
	void ClearLastChatMessage();
};

struct ClientInput
{
public:
	// Input container
	char buffer[512];
	char lastInputBuffer[512];
	int bufferWriteLoc;

	//constructors
	ClientInput() { bufferWriteLoc = 0; };

	// Functions ------------------------------------------

	// adds an ascii value to the input buffer
	// Params
	//	param1 (int): ascii value being put into buffer
	void In(int asciiValue);
	// removes value from last buffer index
	void ClearLastValue();
	// clears lastInputBuffer buffer
	void ClearLastBuffer();
	// clears current input buffer
	void ClearChatBuffer();
};

#endif // !A3_NETAPP_CLIENT_H