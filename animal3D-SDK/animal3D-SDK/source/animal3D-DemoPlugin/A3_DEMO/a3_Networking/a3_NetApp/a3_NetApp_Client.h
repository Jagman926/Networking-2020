#ifndef A3_NETAPP_CLIENT_H
#define A3_NETAPP_CLIENT_H

/* --------------- Text Rendering Variables ----------------------- */

// Input container
char inputBuffer[512];
int bufferLoc = 0;

// Chat container
char chatBuffer[512][512];
int chatLoc = 0;
int chatOffset = 0;
const int CHAT_VIEW_MAX = 22;

/* --------------- Text Rendering Functions ----------------------- */

void OutputToChat(char buffer[]);



#endif // !A3_NETAPP_CLIENT_H