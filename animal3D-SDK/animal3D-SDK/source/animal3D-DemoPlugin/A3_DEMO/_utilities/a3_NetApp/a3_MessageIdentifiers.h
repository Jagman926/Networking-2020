#ifndef RAKNET_CORE_H
#define RAKNET_CORE_H

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"

enum MessageIdentifiers
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	// Custom message identifier for welcoming the player
	ID_GAME_MSG_PLAYER_CONNECTED,
	ID_GAME_MSG_PLAYER_DISCONNECTED,
	ID_CHAT_MSG_BROADCAST,
	ID_CHAT_MSG_REQUEST,
	ID_CHAT_MSG_DELIVERY
};

#endif // !RAKNET_CORE_H
