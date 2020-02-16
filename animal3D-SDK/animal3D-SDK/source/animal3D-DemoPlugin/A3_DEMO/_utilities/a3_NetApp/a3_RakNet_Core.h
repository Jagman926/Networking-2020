#ifndef RAKNET_CORE_H
#define RAKNET_CORE_H

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"
#include <string>



// Custom id's for different message types
enum MessageIdentifiers
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	// Custom message identifier for welcoming the player
	ID_GAME_MSG_PLAYER_CONNECTED,
	ID_GAME_MSG_PLAYER_DISCONNECTED,
	ID_CHAT_MSG_BROADCAST,
	ID_CHAT_MSG_REQUEST,
	ID_CHAT_MSG_DELIVERY,
	ID_KICK,
};

enum UserType
{
	NONE = 0,
	SERVER,
	PLAYER,
	SPECTATOR,
};

struct User
{
public:
	/* ---------- Variables ------------ */

	// Name of user
	char userName[512];
	// User's System Address
	char systemAddress[512];
	// Type of user
	UserType type;
	// Connected to server
	bool isConnected;

	/* --------------------------------- */

	// Default ctor
	User() {};
	// Used for creating new users
	User(char name[], const char* sysAddress, UserType userType);
};

// Custom message packet structure
// Force compiler to pack the structure as byte-aligned.
#pragma pack (push, 1)
struct messagePack
{
public:
	/* VARIABLES */

	RakNet::MessageID msgID;
	// Should use pre allocated byte array memory instead of string
	char msgText[512];

	/* FUNCTIONS */

	messagePack(RakNet::MessageID ID, char msg[]) { msgID = ID, strcpy(msgText, msg); };
	//void CreatePacket(RakNet::MessageID ID, std::string string) { msgID = ID, msgString = string; };

};
#pragma pack (pop)

/*
TODO: Chat message request: Generated by participant, a request for host to send out a message. Should have the following fields (at minimum):
	- Receiver user name: The name of the chat room participant who should receive the message
		- if the message is public (broadcast), use some recognizable keyword or symbol
		- Messaging the host is also allowed; self-messaging is not allowed
	- Message text: The contents of the message.  Should have a maximum length
*/

#pragma pack (push, 1)
struct ChatMessageRequest
{
public:

	// leading byte
	RakNet::MessageID msgID;
	// The name of the chat room participant who should receive the message
	char recieverUserName[512];
	// The contents of the message.  Should have a maximum length.
	char msgTxt[512];


	// Functions
	ChatMessageRequest(RakNet::MessageID ID, const char name[], const char msg[]) { msgID = ID, strcpy(recieverUserName, name), strcpy(msgTxt, msg); };

};
#pragma pack (pop)

/*
TODO: Chat message delivery: Generated by host, an actual message being delivered. Should have the following fields (at minimum):
	- Sender user name: The name of the participant from whom the message originated
		- Since this could include the host, they should be a recognizable name
	- Public or private: Indicates whether the message was broadcast or directed at the receiver
	- Message text: The contents of the message.  Should have a maximum length (same as above)
*/

#pragma pack (push, 1)
struct ChatMessageDelivery
{
public:
	// leading byte
	RakNet::MessageID msgID;
	// The name of the participant from whom the message originated; since this could include the host, they should be a recognizable name.
	char senderUserName[512];
	// Indicates whether the message was broadcast or directed at the receiver.
	bool isPrivate;
	// The contents of the message.  Should have a maximum length.
	char msgTxt[512];

	// Functions
	ChatMessageDelivery(RakNet::MessageID ID, char name[], bool priv, char msg[]) { msgID = ID, strcpy(senderUserName, name), isPrivate = priv, strcpy(msgTxt, msg); };
	//void CreatePacket(RakNet::MessageID ID, std::string string) { msgID = ID, msgString = string; };

};
#pragma pack (pop)

#pragma pack (push, 1)
struct GameDelivery
{
public:
	// leading byte
	RakNet::MessageID msgID;
	// The name of the participant from whom the message originated; since this could include the host, they should be a recognizable name.
	char senderUserName[512];
	// The contents of the message.  Should have a maximum length.
	char msgTxt[512];
	// index for board
	int boardIndex;
	// player turn index
	int playerTurnIndex;
	// challenger 1 name
	char challenger1[512];
	// challenger 2 name
	char challenger2[512];
	
	// Functions
	GameDelivery(RakNet::MessageID ID, char name[], char msg[], int bIndex, int turnIndex, char chal1[], char chal2[]) { msgID = ID, strcpy(senderUserName, name), strcpy(msgTxt, msg), boardIndex = bIndex, playerTurnIndex = turnIndex, strcpy(challenger1, chal1), strcpy(challenger2, chal2); };
	//void CreatePacket(RakNet::MessageID ID, std::string string) { msgID = ID, msgString = string; };

};
#pragma pack (pop)

/* ----------------------- VARIABLES ----------------------------- */


// default max clients
const unsigned int DEFAULT_MAX_CLIENTS = 10;

struct RakClient
{
	// number of maximum clients
	unsigned int maxClients = DEFAULT_MAX_CLIENTS;

	// the server port number
	unsigned short serverPort;
	
	// List containing all users 
	User users[DEFAULT_MAX_CLIENTS];

	// the user to be used by this RakClient
	User thisUser;

	// number of current users, including host
	int currentUsers = 0;

	// string for host system address
	char hostSystemAddress[512];

	// check conncetion
	bool connected = false;

	// Peer to use for this instance of the program
	RakNet::RakPeerInterface* peer;

	// Packet variable to read through in NetUpdate
	RakNet::Packet* packet;	
};

#endif // !RAKNET_CORE_H