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

	//constructors
	ClientChat() { bufferWriteLoc = 0, bufferViewOffset = 0; };

	// Functions
	void In(char buffer[512]);
	void ClearChatBuffer();
};

struct ClientInput
{
public:
	// Input container
	char buffer[512];
	int bufferWriteLoc;

	//constructors
	ClientInput() { bufferWriteLoc = 0; };

	//
	void In(int asciiValue);
	void ClearLastValue();
	void ClearChatBuffer();
};

#endif // !A3_NETAPP_CLIENT_H