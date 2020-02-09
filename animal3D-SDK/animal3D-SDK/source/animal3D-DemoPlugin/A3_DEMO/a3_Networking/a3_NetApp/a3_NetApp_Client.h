#ifndef A3_NETAPP_CLIENT_H
#define A3_NETAPP_CLIENT_H

/* --------------- Text Rendering Variables ----------------------- */

struct ChatText
{
public:
	// const variables
	const int CHAT_VIEW_MAX = 22;
	// Input container
	char inputBuffer[512];
	int bufferLoc = 0;
	// Chat container
	char chatBuffer[512][512];
	int chatLoc = 0;
	int chatOffset = 0;

	//constructors
	ChatText() {};

	//
	void Output(char buffer[512]);
};

#endif // !A3_NETAPP_CLIENT_H