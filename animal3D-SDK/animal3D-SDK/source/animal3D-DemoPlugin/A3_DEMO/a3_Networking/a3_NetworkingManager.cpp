/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_NetworkingManager.cpp
	RakNet networking wrapper.
*/


#include "../a3_NetworkingManager.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/BitStream.h"
#include "RakNet/GetTime.h"


//-----------------------------------------------------------------------------
// networking stuff

enum a3_NetGameMessages
{
	ID_CUSTOM_MESSAGE_START = ID_USER_PACKET_ENUM,

	ID_GAME_MESSAGE_1,
	ID_GAME_MSG_PLAYER_CONNECTED,
	ID_GAME_MSG_PLAYER_DISCONNECTED,
	ID_CHAT_MSG_BROADCAST,
	ID_CHAT_MSG_REQUEST,
	ID_CHAT_MSG_DELIVERY,
	ID_KICK
};

User::User(char name[], const char* sysAddress, UserType userType)
{
	strcpy(userName, name);
	strcpy(systemAddress, sysAddress);
	type = userType;
}


#pragma pack(push, 1)
struct a3_NetGameMessageData
{
	// leading byte
	RakNet::MessageID msgID;
	// The name of the participant from whom the message originated; since this could include the host, they should be a recognizable name.
	char senderUserName[512];
	// The contents of the message.  Should have a maximum length.
	char msgTxt[512];

	// Functions
	a3_NetGameMessageData(RakNet::MessageID ID, char name[], char msg[], int bIndex, int turnIndex, char chal1[], char chal2[]) { msgID = ID, strcpy(senderUserName, name), strcpy(msgTxt, msg); };
};
#pragma pack (pop)

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
};
#pragma pack (pop)

// startup networking
a3i32 a3netStartup(a3_NetworkingManager* net, a3ui16 port_inbound, a3ui16 port_outbound, a3ui16 maxConnect_inbound, a3ui16 maxConnect_outbound)
{
	if (net && !net->peer)
	{
		RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
		if (peer)
		{
			RakNet::SocketDescriptor socket[2] = {
				RakNet::SocketDescriptor(port_inbound, 0),		// server settings
				RakNet::SocketDescriptor(),//port_outbound, 0),	// client settings
			};

			// choose startup settings based on 
			a3boolean const isServer = port_inbound && maxConnect_inbound;
			a3boolean const isClient = port_outbound && maxConnect_outbound;
			// Set isServer
			net->isServer = isServer;
			if (peer->Startup(maxConnect_inbound + maxConnect_outbound, socket + isClient, isServer + isClient) == RakNet::StartupResult::RAKNET_STARTED)
			{
				peer->SetMaximumIncomingConnections(maxConnect_inbound);
				peer->SetOccasionalPing(true);

				net->port_inbound = port_inbound;
				net->port_outbound = port_outbound;
				net->maxConnect_inbound = maxConnect_inbound;
				net->maxConnect_outbound = maxConnect_outbound;
				net->peer = peer;

				return 1;
			}
		}
	}
	return 0;
}

// shutdown networking
a3i32 a3netShutdown(a3_NetworkingManager* net)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		RakNet::RakPeerInterface::DestroyInstance(peer);
		net->peer = 0;
		return 1;
	}
	return 0;
}


// connect
a3i32 a3netConnect(a3_NetworkingManager* net, a3netAddressStr const ip)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		peer->Connect(ip, net->port_outbound, 0, 0);
		return 1;
	}
	return 0;
}

// disconnect
a3i32 a3netDisconnect(a3_NetworkingManager* net)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		a3ui16 i, j = peer->NumberOfConnections();
		for (i = 0; i < j; ++i)
			peer->CloseConnection(peer->GetSystemAddressFromIndex(i), true);
		return 1;
	}
	return 0;
}


// process inbound packets
a3i32 a3netProcessInbound(a3_NetworkingManager* net)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		RakNet::Packet* packet;
		RakNet::MessageID msg;
		a3i32 count = 0;

		for (packet = peer->Receive();
			packet;
			peer->DeallocatePacket(packet), packet = peer->Receive(), ++count)
		{
			RakNet::BitStream bs_in(packet->data, packet->length, false);
			bs_in.Read(msg);
			switch (msg)
			{
				// check for timestamp and process
			case ID_TIMESTAMP:
				bs_in.Read(msg);
				// ****TO-DO: handle timestamp
				RakNet::Time sendTime, recievedTime, dt;
				bs_in.Read(sendTime);
				recievedTime = peer->GetClockDifferential(packet->systemAddress);
				dt = recievedTime - sendTime;
				printf("\n RECIEVED TIME: %u \n", (unsigned int)sendTime);
				printf("\n CLOCKDIFF: %u \n", (unsigned int)recievedTime);
				printf("\n LATENCY: %u \n", (unsigned int)dt);
				// do not break; proceed to default case to process actual message contents
			default:
				switch (msg)
				{
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					printf("Another client has disconnected.\n");
					break;
				case ID_REMOTE_CONNECTION_LOST:
					printf("Another client has lost the connection.\n");
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					printf("Another client has connected.\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					printf("Our connection request has been accepted.\n");
					{
						// Use a BitStream to write a custom user message
						// Bitstreams are easier to use than sending casted structures, 
						//	and handle endian swapping automatically
						RakNet::BitStream bsOut[1];
						RakNet::Time sendTime;
						// indicate timestamp
						bsOut->Write((RakNet::MessageID)ID_TIMESTAMP);
						// get time
						sendTime = RakNet::GetTime();
						bsOut->Write(sendTime);
						// rest of message
						bsOut->Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut->Write("Hello world");
						peer->Send(bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

						// ****TO-DO: write timestamped message
						printf("\n SEND TIME: %u \n", (unsigned int)sendTime);
					}
					break;
				case ID_NEW_INCOMING_CONNECTION:
					printf("A connection is incoming.\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("The server is full.\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					if (net->maxConnect_outbound) {
						printf("A client has disconnected.\n");
					}
					else {
						printf("We have been disconnected.\n");
					}
					break;
				case ID_CONNECTION_LOST:
					if (net->maxConnect_outbound) {
						printf("A client lost the connection.\n");
					}
					else {
						printf("Connection lost.\n");
					}
					break;

				case ID_GAME_MESSAGE_1:
					printf("DEBUG MESSAGE: received packet ID_GAME_MESSAGE_1.\n");
					{
						RakNet::RakString rs;
						bs_in.Read(rs);
						printf("%s\n", rs.C_String());
					}
					break;

				default:
					printf("Message with identifier %i has arrived.\n", msg);
					break;
				}
				break;
			}
		}
		return count;
	}
	return 0;
}

// process outbound packets
a3i32 a3netProcessOutbound(a3_NetworkingManager* net, a3_ChatHandler* chat, char inputMessage [512])
{
	if (net && net->peer)
	{
		// Parsing variables
		char commandDelimiters[] = " /\n\r";
		char messageDelimiters[] = "\n\r";
		// Parsed information
		char* command;
		char* messageTmp;
		char finalMessage[512];
		char finalUser[512];

		// If preceeded with '/', then it is a send command
		if (inputMessage[0] == '/')
		{
			// Parse for command name
			command = strtok(inputMessage, commandDelimiters);
			// Save message text
			messageTmp = strtok(NULL, messageDelimiters);

			// ------------------------------- Client Commands ------------------------------------

			// -------- All Message ---------
			if (strcmp(command, "all") == 0)
			{
				// Get final message
				strncpy(finalMessage, messageTmp, 512);

				// If not host, send to host
				if (net->isServer)
				{
					// Create our message request
					ChatMessageRequest messageRequest(ID_CHAT_MSG_REQUEST, "all", finalMessage);
					net->peer->Send((const char*)& messageRequest, sizeof(messageRequest), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)rakClient.thisUser.systemAddress, false);
				}
				// If host, Send broadcast
				else
				{
					// Create our message delivery
					ChatMessageDelivery msgDelivery(ID_CHAT_MSG_DELIVERY, rakClient.thisUser.userName, false, finalMessage);
					rakClient.peer->Send((const char*)& msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)rakClient.thisUser.systemAddress, true);
				}
			}
			// ------ Private Message -------
			else if (strcmp(command, "msg") == 0)
			{
				char* userTemp;
				// getUsersName
				userTemp = strtok(messageTmp, commandDelimiters);
				// Save message text
				messageTmp = strtok(NULL, messageDelimiters);
				// Get final message and user
				strncpy(finalMessage, messageTmp, 512);
				strncpy(finalUser, userTemp, 512);

				// If not host, send to host
				if (net->isServer)
				{
					// Create our message request
					ChatMessageRequest messageRequest(ID_CHAT_MSG_REQUEST, finalUser, finalMessage);
					rakClient.peer->Send((const char*)& messageRequest, sizeof(messageRequest), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)rakClient.hostSystemAddress, false);
				}
				// If host, Send broadcast
				else
				{
					char recieverSystemAddress[512];
					// read through user database
					for (int i = 0; i < rakClient.currentUsers; i++)
					{
						if (strcmp(rakClient.users[i].userName, finalUser) == 0)
							strcpy(recieverSystemAddress, rakClient.users[i].systemAddress);
					}

					// Create our message delivery
					ChatMessageDelivery msgDelivery(ID_CHAT_MSG_DELIVERY, rakClient.thisUser.userName, true, finalMessage);
					rakClient.peer->Send((const char*)& msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)recieverSystemAddress, false);
				}
			}
			// ------ Clear Chat History -------
			else if (strcmp(command, "clear") == 0)
			{
				cChat.ClearChatBuffer();
			}
			// ----------- Help ------------
			else if (strcmp(command, "help") == 0)
			{
				if (rakClient.thisUser.type == UserType::SERVER)
				{
					cChat.In("Admin Commands:");
					cChat.In("   /users: output all usernames and corresponding system address");
					cChat.In("   /kick [username]: kicks specified user from lobby");

					cChat.In("Game Commands:");
					cChat.In("   /challenge [user1] [user2] [TicTacToe/Checkers]: start an online game");
					cChat.In("   /play [TicTacToe/Checkers]: starts a local game");
				}
				cChat.In("Client Commands:");
				cChat.In("   /all: send message to all members of chat");
				cChat.In("   /msg [username]: send private message to user");
				cChat.In("   /clear: clear chat history");
				cChat.In("   /exit: disconnect from server");

				cChat.In("Game Commands:");
				cChat.In("   /ttt [0-8]: mark space on TicTacToe board");

			}


			// Tic Tac Toe 
			else if (strcmp(command, "ttt") == 0)
			{
				// online
				if (!gameInstance.isLocal)
				{
					if (strcmp(rakClient.thisUser.userName, gameInstance.players[gameInstance.ticTacToe.turnIndex]) == 0)
					{
						char finalNum[512];

						char* temp;

						// get number to enter into the board state
						temp = strtok(messageTmp, commandDelimiters);
						strncpy(finalNum, temp, 512);

						// set the space based on inputted number
						gameInstance.ticTacToe.SetSpace(atoi(finalNum), gameInstance.ticTacToe.GetPlayerSpaceType());
						// swap turn
						gameInstance.ticTacToe.NextTurn();

						char recieverSystemAddress[512];
						// read through user database
						for (int i = 0; i < rakClient.currentUsers; i++)
						{
							if (strcmp(rakClient.users[i].userName, gameInstance.players[gameInstance.ticTacToe.turnIndex]) == 0)
								strcpy(recieverSystemAddress, rakClient.users[i].systemAddress);
						}

						// Send the ENTIRE game 
						GameDelivery gameDelivery(ID_GAME_DELIVERY, rakClient.thisUser.userName, "TicTacToe: Its your turn", atoi(finalNum), gameInstance.ticTacToe.turnIndex, gameInstance.players[0], gameInstance.players[1]);
						rakClient.peer->Send((const char*)& gameDelivery, sizeof(gameDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)recieverSystemAddress, false);

					}
					else
					{
						cChat.In("It is not your turn. Please wait!");
					}
				}
				// Local game
				else
				{
					char finalNum[512];

					char* temp;

					// get number to enter into the board state
					temp = strtok(messageTmp, commandDelimiters);
					strncpy(finalNum, temp, 512);

					// set the space based on inputted number
					gameInstance.ticTacToe.SetSpace(atoi(finalNum), gameInstance.ticTacToe.GetPlayerSpaceType());
					// swap turn
					gameInstance.ticTacToe.NextTurn();

					char msgFormat[512];

					// Display next player
					sprintf(msgFormat, "%s, its your turn!", gameInstance.players[gameInstance.ticTacToe.turnIndex]);
					cChat.In(msgFormat);
				}

			}

			// -------- Disconnect ---------
			else if (strcmp(command, "exit") == 0)
			{
				// Create data structure and initialize with our message identifier and message, in this case the user inputted client name
				messagePack pack(ID_GAME_MSG_PLAYER_DISCONNECTED, rakClient.thisUser.userName);
				// Send the data structure to the server by casting it to a byte stream using const char* and passing the size of our data structure
				rakClient.peer->Send((const char*)& pack, sizeof(pack), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)rakClient.hostSystemAddress, false);
				// Init lobby
				a3DemoNetworking_lobby_init();
			}

			// ------------------------------- Host Only Commands ------------------------------------

			// -------- User Output --------
			else if (rakClient.thisUser.type == UserType::SERVER && strcmp(command, "users") == 0)
			{
				for (int i = 0; i < rakClient.currentUsers; i++)
				{
					char msgFormat[512];
					sprintf(msgFormat, "User: %s | Address: %s", rakClient.users[i].userName, rakClient.users[i].systemAddress);
					cChat.In(msgFormat);
				}
			}
			// ------- Kick --------
			else if (rakClient.thisUser.type == UserType::SERVER && strcmp(command, "kick") == 0)
			{
				// name of player to kick
				char finalUser[512];

				char* temp;

				// get final challenger name to enter
				temp = strtok(messageTmp, commandDelimiters);
				strncpy(finalUser, temp, 512);

				char userAddress[512];

				// read through user database
				for (int i = 0; i < rakClient.currentUsers; i++)
				{
					// check for user 1
					if (strcmp(rakClient.users[i].userName, finalUser) == 0)
						strcpy(userAddress, rakClient.users[i].systemAddress);
				}



				// Send to all users
				messagePack msgKick(ID_KICK, "kick");
				rakClient.peer->Send((const char*)& msgKick, sizeof(msgKick), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID) userAddress, false);

			}

			// Challenge players to online games
			else if (rakClient.thisUser.type == UserType::SERVER && strcmp(command, "challenge") == 0)
			{
				// the user name of the first challenger to challenge
				char finalChallenger1[512];
				// the user name of the first challenger to challenge
				char finalChallenger2[512];
				// selects the game to be played 
				char finalGame[512];


				char* temp;

				// get final challenger name to enter
				temp = strtok(messageTmp, commandDelimiters);
				strncpy(finalChallenger1, temp, 512);

				// get second final challenger name to enter
				temp = strtok(NULL, commandDelimiters);
				strncpy(finalChallenger2, temp, 512);

				// get game name to enter
				temp = strtok(NULL, commandDelimiters);
				strncpy(finalGame, temp, 512);

				char challenger1Address[512];
				char challenger2Address[512];

				// read through user database
				for (int i = 0; i < rakClient.currentUsers; i++)
				{
					// check for user 1
					if (strcmp(rakClient.users[i].userName, finalChallenger1) == 0)
						strcpy(challenger1Address, rakClient.users[i].systemAddress);
					// check for user 2
					if (strcmp(rakClient.users[i].userName, finalChallenger2) == 0)
						strcpy(challenger2Address, rakClient.users[i].systemAddress);
				}


				// start a game of TicTacToe
				if (strcmp(finalGame, "TicTacToe") == 0)
				{
					// Init the game with our challenger names
					gameInstance = Game(finalChallenger1, finalChallenger2, false, GameType::TICTACTOE);

					// check if a game instance is running
					if (gameInstance.IsPlayer(rakClient.thisUser.userName))
					{
						gameInstance.PlayerReady(rakClient.thisUser.userName, true);
						cChat.In("Game started");
						gameInstance.inProgress = true;
						// randomize 
						gameInstance.ticTacToe.turnIndex = a3randomMaxInt(1);

					}
					else
					{
						cChat.In("You are not a player in an active game session!");
					}
				}
				// start a game of Checkers
				else if (strcmp(finalGame, "Checkers") == 0)
				{
					// Init the game with our challenger names
					gameInstance = Game(finalChallenger1, finalChallenger2, false, GameType::CHECKERS);
				}

				char msgFormat[512];
				sprintf(msgFormat, "%s and %s will play %s! %s goes first!", finalChallenger1, finalChallenger2, finalGame, gameInstance.players[gameInstance.ticTacToe.turnIndex]);
				cChat.In(msgFormat);


				// Create our message delivery for the challenge
				GameDelivery msgDelivery(ID_GAME_CHALLENGE, rakClient.thisUser.userName, msgFormat, -1, gameInstance.ticTacToe.turnIndex, finalChallenger1, finalChallenger2);
				rakClient.peer->Send((const char*)& msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)challenger2Address, false);


				// challenge -> playerName -> game
			}

			//----------------GAME STATUS----------------//
			else if (rakClient.thisUser.type == UserType::SERVER && strcmp(command, "gameStatus") == 0)
			{
				char msgFormat[512];
				// Local game status
				if (gameInstance.isLocal)
				{
					if (gameInstance.type == GameType::TICTACTOE)
					{
						sprintf(msgFormat, "Local TicTacToe");
						cChat.In(msgFormat);
					}
					else if (gameInstance.type == GameType::CHECKERS)
					{
						sprintf(msgFormat, "Local Checkers");
						cChat.In(msgFormat);
					}
				}
				// Online game status
				else if (gameInstance.isLocal == false)
				{
					if (gameInstance.type == GameType::TICTACTOE)
					{
						sprintf(msgFormat, "Online TicTacToe");
						cChat.In(msgFormat);
						sprintf(msgFormat, "%s | Ready: %i", gameInstance.players[0], gameInstance.ready[0]);
						cChat.In(msgFormat);
						sprintf(msgFormat, "%s | Ready: %i", gameInstance.players[1], gameInstance.ready[1]);
						cChat.In(msgFormat);
					}
					else if (gameInstance.type == GameType::CHECKERS)
					{
						sprintf(msgFormat, "Online Checkers");
						cChat.In(msgFormat);
						sprintf(msgFormat, "%s | Ready: %i", gameInstance.players[0], gameInstance.ready[0]);
						cChat.In(msgFormat);
						sprintf(msgFormat, "%s | Ready: %i", gameInstance.players[1], gameInstance.ready[1]);
						cChat.In(msgFormat);
					}
					else
					{
						cChat.In("No game has been created");
					}
				}

			}



			// Play the game locally
			else if (rakClient.thisUser.type == UserType::SERVER && strcmp(command, "play") == 0)
			{
				char msgFormat[512];

				// selects the game to be played 
				char finalGame[512];

				char* temp;

				// get game name to enter
				temp = strtok(messageTmp, commandDelimiters);
				strncpy(finalGame, temp, 512);

				// pass input
				if (strcmp(finalGame, "TicTacToe") == 0)
				{
					cChat.In("TicTacToe by yourself!");
					gameInstance = Game("Player 1", "Player 2", true, GameType::TICTACTOE);
					gameInstance.inProgress = true;
					// set random start
					gameInstance.ticTacToe.turnIndex = a3randomMaxInt(1);
					sprintf(msgFormat, "%s goes first!", gameInstance.players[gameInstance.ticTacToe.turnIndex]);
					cChat.In(msgFormat);
				}
				else if (strcmp(finalGame, "Checkers") == 0)
				{
					cChat.In("Checkers by yourself!");
					gameInstance = Game("Player 1", "Player 2", true, GameType::CHECKERS);
					gameInstance.inProgress = true;
				}

			}

			// Invalid command
			else
			{
				char msgFormat[512];
				sprintf(msgFormat, "%s is an invalid command, type /help to see valid commands", command);
				cChat.In(msgFormat);
			}
		}
	}
	return 0;
}


//-----------------------------------------------------------------------------
