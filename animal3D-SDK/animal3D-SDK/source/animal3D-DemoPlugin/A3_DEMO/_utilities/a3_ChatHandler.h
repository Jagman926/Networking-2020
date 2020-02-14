#ifndef CHAT_HANDLER_H
#define CHAT_HANDLER_H

//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ChatHandler				a3_ChatHandler;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// input handler
	struct a3_ChatHandler
	{
		// variables
		int chatViewMax;
		int hideChat;
		int unreadMessages;
		// Chat container
		char buffer[512][512];
		int bufferWriteLoc;
		int bufferViewOffset;
};

//-----------------------------------------------------------------------------

	void InitChat(a3_ChatHandler* chat);

	// adds buffer to chat buffer at current write location
	// Params
	//	param1 (char[512]): buffer input
	void ChatIn(a3_ChatHandler *chat, char buffer[512]);

	// clears entire chat buffer
	void ClearChatBuffer(a3_ChatHandler* chat);

	// clears last inputted chat buffer
	void ClearLastChatMessage(a3_ChatHandler* chat);

//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // !CHAT_HANDLER_H

