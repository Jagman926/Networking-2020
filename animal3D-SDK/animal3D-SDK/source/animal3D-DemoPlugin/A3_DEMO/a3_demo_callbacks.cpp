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
	
	a3_demo_callbacks.c/.cpp
	Main implementation of window callback hooks.

	********************************************
	*** THIS IS THE LIBRARY'S CALLBACKS FILE ***
	*** App hooks your demo via this file.   ***
	********************************************
*/

// Animal Includes

#include "a3_dylib_config_export.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"

#include "animal3D-A3DM/animal3D-A3DM.h"

#include <GL/glew.h>

// Client Includes
#include "A3_DEMO/a3_Networking/a3_NetApp/a3_NetApp_Client.h"

// Network Includes
#include "A3_DEMO/_utilities/a3_NetApp/a3_RakNet_Core.h"

// Game Includes
#include "A3_DEMO/a3_Networking/a3_NetApp/a3_NetApp_Game.h"


struct a3_DemoState
{
	//---------------------------------------------------------------------
	// general variables pertinent to the state

	// terminate key pressed
	a3i32 exitFlag;

	// global vertical axis: Z = 0, Y = 1
	a3i32 verticalAxis;

	// asset streaming between loads enabled (careful!)
	a3i32 streaming;

	// window and full-frame dimensions
	a3ui32 windowWidth, windowHeight;
	a3real windowWidthInv, windowHeightInv, windowAspect;
	a3ui32 frameWidth, frameHeight;
	a3real frameWidthInv, frameHeightInv, frameAspect;
	a3i32 frameBorder;


	//---------------------------------------------------------------------
	// objects that have known or fixed instance count in the whole demo

	// text renderer
	a3i32 textInit, textMode, textModeCount;
	a3_TextRenderer text[1];

	// input
	a3_MouseInput mouse[1];
	a3_KeyboardInput keyboard[1];
	a3_XboxControllerInput xcontrol[4];

	// pointer to fast trig table
	a3f32 trigTable[4096 * 4];

	a3_Timer renderTimer[1];

};


// ------------------GAME----------------------//
Game gameInstance;


// Multi-Instance Notes
// main_win.c
// line 70 is single instance
// line 72-73 is multi instance

// Client Variable Structures ----------------------------------------------------------

ClientChat cChat;
ClientInput cInput;
RakClient rakClient;
// Lobby Prompts ---------------------------------------------------------

char userNamePrompt[512];
char userTypePrompt[512];
char ipConnectPrompt[512];

void a3DemoNetworking_lobby_init()
{
	// Set connected to false
	rakClient.connected = false;

	// Reset lobby prompts
	strcpy(userNamePrompt, "Enter Username");
	strcpy(userTypePrompt, "Would you like to Join (J) or Host (H) a chat room? (D) to Disconnect");
	strcpy(ipConnectPrompt, "Enter server IP or (L) for 127.0.0.1");

	// Reset username and usertype (inputted from prompts in lobby)
	memset(rakClient.thisUser.userName, 0, sizeof rakClient.thisUser.userName);
	rakClient.thisUser.type = UserType::NONE;

	// Clear any remaining chat history from previous chats
	cChat.ClearChatBuffer();
}

void a3DemoNetworking_init()
{
	if (!rakClient.connected)
	{
		
		if (rakClient.thisUser.userName[0] == 0)
		{
			if (userNamePrompt[0] != 0)
			{
				cChat.In(userNamePrompt);
				memset(userNamePrompt, 0, sizeof userNamePrompt);
			}
			else if (cInput.lastInputBuffer[0] != 0)
			{
				// Parse for command name
				char* tempUser = strtok(cInput.lastInputBuffer, "\r");
				strncpy(rakClient.thisUser.userName, tempUser, 512);
			}
		}
		else if (rakClient.thisUser.type == UserType::NONE)
		{
			if (userTypePrompt[0] != 0)
			{
				cChat.In(userTypePrompt);
				memset(userTypePrompt, 0, sizeof userTypePrompt);
			}
			else if(cInput.lastInputBuffer != 0)
			{
				if (cInput.lastInputBuffer[0] == 'h' || cInput.lastInputBuffer[0] == 'H')
				{
					// Set type as player
					rakClient.thisUser.type = UserType::SERVER;

					// init the server port
					rakClient.serverPort = 1;

					// Start the Peer
					RakNet::SocketDescriptor sd(rakClient.serverPort, 0);
					rakClient.peer->Startup(rakClient.maxClients, &sd, 1);

					// Create the User representing the server
					User server(rakClient.thisUser.userName, rakClient.peer->GetSystemAddressFromGuid(rakClient.peer->GetMyGUID()).ToString(), SERVER);

					// Add the server to the user list
					rakClient.users[rakClient.currentUsers] = server;

					// increment current users now that we have one
					rakClient.currentUsers++;

					// Set this rakclient to be a server
					rakClient.thisUser = server;

				}
				else if (cInput.lastInputBuffer[0] == 'j' || cInput.lastInputBuffer[0] == 'J')
				{
					// Set type as player
					rakClient.thisUser.type = UserType::PLAYER;

					// Start the Peer
					RakNet::SocketDescriptor sd;
					rakClient.peer->Startup(1, &sd, 1);

					// Create the user for the player
					User player(rakClient.thisUser.userName, rakClient.peer->GetSystemAddressFromGuid(rakClient.peer->GetMyGUID()).ToString(), PLAYER);

					// init the server port
					rakClient.serverPort = 1;

					// Set this rakclient to be a player
					rakClient.thisUser = player;
				}
				else if (cInput.lastInputBuffer[0] == 's' || cInput.lastInputBuffer[0] == 'S')
				{
					rakClient.thisUser.type = UserType::SPECTATOR;

				}
				else if (cInput.lastInputBuffer[0] == 'd' || cInput.lastInputBuffer[0] == 'D')
				{
					// TO-DO close application
				}
			}
		}
		else
		{
			// Server initialization
			if (rakClient.thisUser.type == SERVER)
			{
				// Clear chat
				cChat.ClearChatBuffer();
				// Notify starting server
				cChat.In("Server Started!");
				// Set connected
				rakClient.connected = true;
				// We need to let the server accept incoming connections from the clients
				rakClient.peer->SetMaximumIncomingConnections(rakClient.maxClients);
			}
			// Player initialization
			else
			{
				if (ipConnectPrompt[0] != 0)
				{
					cChat.In(ipConnectPrompt);
					memset(ipConnectPrompt, 0, sizeof ipConnectPrompt);
				}
				else if (cInput.lastInputBuffer[0] != 0)
				{
					if (cInput.lastInputBuffer[0] == 'L' || cInput.lastInputBuffer[0] == 'l')
					{
						// Set host system address
						strcpy(rakClient.hostSystemAddress,"127.0.0.1|1");
						// Connect to host
						rakClient.peer->Connect("127.0.0.1", rakClient.serverPort, 0, 0);
					}
					else
					{
						// Set host system address
						char messageFormat[512];
						sprintf(messageFormat, "%s|1", cInput.lastInputBuffer);
						strcpy(rakClient.hostSystemAddress, messageFormat);
						// Connect to host
						rakClient.peer->Connect(cInput.lastInputBuffer, rakClient.serverPort, 0, 0);
					}
					// Clear chat
					cChat.ClearChatBuffer();
					// Notify starting client
					cChat.In("Client Starting!");
					// Set connected
					rakClient.connected = true;
				}
			}
		}
	}

}

void a3DemoNetworking_recieve()
{
	// For loop for reading packet information
	for (rakClient.packet = rakClient.peer->Receive(); rakClient.packet; rakClient.peer->DeallocatePacket(rakClient.packet), rakClient.packet = rakClient.peer->Receive())
	{
		// Message format array for output
		char msgFormat[512];

		switch (rakClient.packet->data[0])
		{
			/* ------------------------------------------------------------------------------------------------------- */
			/*                                            DEFAULT MESSAGES                                             */
			/* ------------------------------------------------------------------------------------------------------- */

		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			cChat.In("Another client has disconnected");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			cChat.In("Another client has lost the connection");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			cChat.In("Another client has connected");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			cChat.In("Our connection request has been accepted");
			// Create data structure and initialize with our message identifier and message, in this case the user inputted client name
			messagePack pack(ID_GAME_MSG_PLAYER_CONNECTED, rakClient.thisUser.userName);
			// Send the data structure to the server by casting it to a byte stream using const char* and passing the size of our data structure
			rakClient.peer->Send((const char*)& pack, sizeof(pack), HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakClient.packet->systemAddress, false);
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			cChat.In("A connection is incoming");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			cChat.In("The server is full");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (rakClient.thisUser.type == SERVER) {
				cChat.In("A client has disconnected");
			}
			else {
				cChat.In("We have been disconnected");
			}
			break;
		case ID_CONNECTION_LOST:
			if (rakClient.thisUser.type == SERVER) {
				cChat.In("A client lost the connection");
			}
			else {
				cChat.In("Connection lost");
			}
			break;

			/* ------------------------------------------------------------------------------------------------------- */
			/*                                            CUSTOM MESSAGES                                              */
			/* ------------------------------------------------------------------------------------------------------- */

		// Custom message loop for player welcome
		case ID_GAME_MSG_PLAYER_CONNECTED:
		{
			// Cast packet to our data structure
			messagePack* p = (messagePack*)rakClient.packet->data;

			// Create the client
			User client(p->msgText, rakClient.packet->systemAddress.ToString(), UserType::PLAYER);

			// Add the client to the user list
			rakClient.users[rakClient.currentUsers] = client;

			// increment current users
			rakClient.currentUsers++;

			// Print the message with the message string from the structure
			sprintf(msgFormat, "%s has joined the chat!", p->msgText);
			cChat.In(msgFormat);

			// Send to all users
			messagePack msgDelivery(ID_CHAT_MSG_BROADCAST, msgFormat);
			rakClient.peer->Send((const char*)& msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakClient.packet->systemAddress, true);
		}
		break;

		// Custom message loop for player disconnect
		case ID_GAME_MSG_PLAYER_DISCONNECTED:
		{
			// Cast packet to our data structure
			messagePack* p = (messagePack*)rakClient.packet->data;
			// Print the message with the message string from the structure
			sprintf(msgFormat, "%s has left the chat", p->msgText);
			cChat.In(msgFormat);

			// Send to all users
			messagePack msgDelivery(ID_CHAT_MSG_BROADCAST, msgFormat);
			rakClient.peer->Send((const char*)& msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakClient.packet->systemAddress, true);
		}
		break;
		// DAN
		case ID_CHAT_MSG_REQUEST:
		{
			// cast packet to data
			ChatMessageRequest* p = (ChatMessageRequest*)rakClient.packet->data;

			// Name to designate in the message delivery
			char senderName[512];
			char recieverSystemAddress[512];
			// read through user database
			for (int i = 0; i < rakClient.currentUsers; i++)
			{
				// check the system address against the one in the packet
				if (strcmp(rakClient.users[i].systemAddress, rakClient.packet->systemAddress.ToString()) == 0)
				{
					// copy it on over
					strcpy(senderName, rakClient.users[i].userName);
				}
				if (strcmp(rakClient.users[i].userName, p->recieverUserName) == 0)
				{
					strcpy(recieverSystemAddress, rakClient.users[i].systemAddress);
				}
			}

			// check if we broadcasting
			if (strcmp(p->recieverUserName, "all") == 0)
			{
				// Output message for host
				sprintf(msgFormat, "(All) %s: %s", senderName, p->msgTxt);
				cChat.In(msgFormat);
				// build message delivery data
				ChatMessageDelivery msgDelivery(ID_CHAT_MSG_DELIVERY, senderName, false, p->msgTxt);
				rakClient.peer->Send((const char*)& msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakClient.packet->systemAddress, true);
			}
			else
			{
				// Check if self send private message
				if (strcmp(senderName, p->recieverUserName) == 0)
				{
					// Output message for host
					sprintf(msgFormat, "(Error) %s tried to send a private message to themself", senderName);
					cChat.In(msgFormat);
				}
				else
				{
					// Output message for host
					if (strcmp(recieverSystemAddress, rakClient.thisUser.systemAddress))
					{
						sprintf(msgFormat, "(Private) %s to %s: %s", senderName, p->recieverUserName, p->msgTxt);
						cChat.In(msgFormat);
					}
					// build message delivery data
					ChatMessageDelivery msgDelivery(ID_CHAT_MSG_DELIVERY, senderName, true, p->msgTxt);
					rakClient.peer->Send((const char*)& msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)recieverSystemAddress, false);
				}
			}
		}
		break;
		case ID_CHAT_MSG_DELIVERY:
		{
			// cast packet to data
			ChatMessageDelivery* p = (ChatMessageDelivery*)rakClient.packet->data;
			if (p->isPrivate)
			{
				sprintf(msgFormat, "(Private) %s: %s", p->senderUserName, p->msgTxt);
				cChat.In(msgFormat);
			}
			else
			{
				sprintf(msgFormat, "%s: %s", p->senderUserName, p->msgTxt);
				cChat.In(msgFormat);
			}
		}
		break;

		case ID_CHAT_MSG_BROADCAST:
		{
			// Cast packet to our data structure
			messagePack* p = (messagePack*)rakClient.packet->data;
			// Print the message with the message string from the structure
			cChat.In(p->msgText);
		}
		break;

		case ID_GAME_CHALLENGE:
		{
			// Packet to data struct
			ChatMessageDelivery* p = (ChatMessageDelivery*)rakClient.packet->data;
			// Print the message with the message string from the structure
			cChat.In(p->msgTxt);

			break;
		}

		default:
			sprintf(msgFormat, "Message with identifier %i has arrived", rakClient.packet->data[0]);
			cChat.In(msgFormat);
			break;
		}

		// Erase information of msgFormat
		memset(msgFormat, 0, sizeof msgFormat);
	}
}

void a3DemoNetworking_send(char message [512]) 
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
	if (cInput.lastInputBuffer[0] == '/')
	{
		// Parse for command name
		command = strtok(message, commandDelimiters);
		// Save message text
		messageTmp = strtok(NULL, messageDelimiters);
		
		// ------------------------------- Client Commands ------------------------------------

		// -------- All Message ---------
		if (strcmp(command, "all") == 0)
		{
			// Get final message
			strncpy(finalMessage, messageTmp, 512);

			// If not host, send to host
			if (rakClient.thisUser.type != UserType::SERVER)
			{
				// Create our message request
				ChatMessageRequest messageRequest(ID_CHAT_MSG_REQUEST, "all", finalMessage);
				rakClient.peer->Send((const char*)& messageRequest, sizeof(messageRequest), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)rakClient.hostSystemAddress, false);
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
			if (rakClient.thisUser.type != UserType::SERVER)
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
			}
			cChat.In("Client Commands:");
			cChat.In("   /all: send message to all members of chat");
			cChat.In("   /msg [username]: send private message to user");
			cChat.In("   /clear: clear chat history");
			cChat.In("   /exit: disconnect from server");
		}

		// ----------- Ready ------------
		else if (strcmp(command, "ready") == 0)
		{
			// check if a game instance is running
			if (gameInstance.IsPlayer(rakClient.thisUser.userName))
			{
				gameInstance.PlayerReady(rakClient.thisUser.userName, true);
				cChat.In("You are now ready!");
			}
			else
			{
				cChat.In("You are not a player in an active game session!");
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

			char msgFormat[512];
			sprintf(msgFormat, "%s and %s will play %s! Enter /ready to start!", finalChallenger1, finalChallenger2, finalGame);
			cChat.In(msgFormat);
			 
			// Create our message delivery for the challenge
			ChatMessageDelivery msgDelivery(ID_GAME_CHALLENGE, rakClient.thisUser.userName, false, msgFormat);
			rakClient.peer->Send((const char*)&msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)rakClient.thisUser.systemAddress, true);
			
			// start a game of TicTacToe
			if (strcmp(finalGame, "TicTacToe"))
			{
				// Init the game with our challenger names
				gameInstance = Game(finalChallenger1, finalChallenger2, false, GameType::TICTACTOE);
			}
			// start a game of Checkers
			else if (strcmp(finalGame, "Checkers"))
			{
				// Init the game with our challenger names
				gameInstance = Game(finalChallenger1, finalChallenger2, false, GameType::CHECKERS);
			}
		


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
		}
		else if (strcmp(finalGame, "Checkers") == 0)
		{
			cChat.In("Checkers by yourself!");
			gameInstance = Game("Player 1", "Player 2", true, GameType::CHECKERS);

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

void a3DemoRenderTextChat(a3_DemoState const* demostate)
{
	// amount to offset text as each line is rendered
	a3f32 textAlign = -0.98f;
	a3f32 textOffset = 1.00f;
	a3f32 textDepth = -1.00f;
	const a3f32 textOffsetDelta = -0.08f;

	// Renders chat log
	int i;
	for (i = cChat.bufferViewOffset; i < cChat.bufferViewOffset + cChat.CHAT_VIEW_MAX; i++)
	{
		if (i >= 0 && i < cChat.bufferWriteLoc)
		{
			a3textDraw(demostate->text, textAlign, textOffset += textOffsetDelta, textDepth, 1, 1, 1, 1, "%s", cChat.buffer[i]);
		}
	}

	// Renders users typebox
	a3textDraw(demostate->text, -0.98f, -0.95f, -1.0f, 1, 1, 1, 1, cInput.buffer);
}

void a3DemoRenderClient(a3_DemoState const* demoState)
{
	// clear color
	glClear(GL_COLOR_BUFFER_BIT);
	// render chat
	a3DemoRenderTextChat(demoState);
}

void a3DemoUpdate(a3_DemoState const* demoState) 
{
	// Update Order
	/* 
	1. Input (done in idle before this is called)
	2. RakNet Update
		1. Init (if not connected)
		2. Packet Handling 
		3. Input Parse -> Send
	3. Render Client
	4. Clear last input buffer
	*/

	if (!rakClient.connected)
		a3DemoNetworking_init();
	else
	{
		// Packet handling
		a3DemoNetworking_recieve();
		// Input parse and Send
		a3DemoNetworking_send(cInput.lastInputBuffer);
	}

	/*
	// Happens when player wants to disconnect from server
	rakClient.peer->Shutdown(50, 0, LOW_PRIORITY);
	// Destroy peer on disconnect (Happens before client closes)
	RakNet::RakPeerInterface::DestroyInstance(rakClient.peer);
	*/

	// Render all text for screen
	a3DemoRenderClient(demoState);
	// Clear last buffer input (the input that was entered this frame)
	cInput.ClearLastBuffer();
}

//-----------------------------------------------------------------------------
// miscellaneous functions

// get the size of the persistent state to allocate
//	(good idea to keep it relatively constant, so that addresses don't change 
//	when the library is reloaded... that would mess everything up!)
inline a3ui32 a3demo_getPersistentStateSize()
{
	const a3ui32 minimum = sizeof(a3_DemoState);
	a3ui32 size = 1;
	while (size < minimum)
		size += size;
	return size;
}


// consistent text initialization
inline void a3demo_initializeText(a3_TextRenderer *text)
{
	a3textInitialize(text, 18, 1, 0, 0, 0);
}


//-----------------------------------------------------------------------------
// callback sub-routines

inline void a3demoCB_keyCharPress_main(a3_DemoState *demoState, a3i32 asciiKey,
	const a3ui32 demoSubMode, const a3ui32 demoOutput,
	const a3ui32 demoSubModeCount, const a3ui32 demoOutputCount)
{

}

inline void a3demoCB_keyCharHold_main(a3_DemoState *demoState, a3i32 asciiKey)
{

}


//-----------------------------------------------------------------------------
// callback prototypes
// NOTE: do not move to header; they should be private to this file
// NOTE: you may name these functions whatever you like, just be sure to 
//	update your debug config file: 
//	"<root>/resource/animal3D-data/animal3D-demoinfo-debug.txt"
// copy this config line and the DLL to your main config with a new name when 
//	you're happy with it: 
//	"<root>/resource/animal3D-data/animal3D-demoinfo.txt"


#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	A3DYLIBSYMBOL a3_DemoState *a3demoCB_load(a3_DemoState *demoState, a3boolean hotbuild);
	A3DYLIBSYMBOL a3_DemoState *a3demoCB_unload(a3_DemoState *demoState, a3boolean hotbuild);
	A3DYLIBSYMBOL a3i32 a3demoCB_display(a3_DemoState *demoState);
	A3DYLIBSYMBOL a3i32 a3demoCB_idle(a3_DemoState *demoState);
	A3DYLIBSYMBOL void a3demoCB_windowActivate(a3_DemoState *demoState);
	A3DYLIBSYMBOL void a3demoCB_windowDeactivate(a3_DemoState *demoState);
	A3DYLIBSYMBOL void a3demoCB_windowMove(a3_DemoState *demoState, a3i32 newWindowPosX, a3i32 newWindowPosY);
	A3DYLIBSYMBOL void a3demoCB_windowResize(a3_DemoState *demoState, a3i32 newWindowWidth, a3i32 newWindowHeight);
	A3DYLIBSYMBOL void a3demoCB_keyPress(a3_DemoState *demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyHold(a3_DemoState *demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyRelease(a3_DemoState *demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyCharPress(a3_DemoState *demoState, a3i32 asciiKey);
	A3DYLIBSYMBOL void a3demoCB_keyCharHold(a3_DemoState *demoState, a3i32 asciiKey);
	A3DYLIBSYMBOL void a3demoCB_mouseClick(a3_DemoState *demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseDoubleClick(a3_DemoState *demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseRelease(a3_DemoState *demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseWheel(a3_DemoState *demoState, a3i32 delta, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseMove(a3_DemoState *demoState, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseLeave(a3_DemoState *demoState);

#ifdef __cplusplus
}
#endif	// __cplusplus


//-----------------------------------------------------------------------------
// callback implementations

// demo is loaded
A3DYLIBSYMBOL a3_DemoState *a3demoCB_load(a3_DemoState *demoState, a3boolean hotbuild)
{
	const a3ui32 stateSize = a3demo_getPersistentStateSize();
	const a3ui32 trigSamplesPerDegree = 4;
	
	// do any re-allocation tasks
	if (demoState && hotbuild)
	{
		/*

		const a3ui32 stateSize = a3demo_getPersistentStateSize();
		a3_DemoState copy = *demoState;

		// example 1: copy memory directly
		free(demoState);
		demoState = (a3_DemoState *)malloc(stateSize);
		memset(demoState, 0, stateSize);
		*demoState = copy;

		a3trigInitSetTables(trigSamplesPerDegree, demoState->trigTable);
		
		// call refresh to re-link pointers in case demo state address changed
		a3demo_refresh(demoState);
		a3demo_initSceneRefresh(demoState);

		*/
	}

	// do any initial allocation tasks
	else
	{
		// HEAP allocate persistent state
		// stack object will be deleted at the end of the function
		// good idea to set the whole block of memory to zero
		demoState = (a3_DemoState *)malloc(stateSize);
		
		if (demoState)
		{
			memset(demoState, 0, stateSize);

			// set up trig table (A3DM)
			a3trigInit(trigSamplesPerDegree, demoState->trigTable);

			// initialize state variables
			// e.g. timer, thread, etc.
			a3timerSet(demoState->renderTimer, 30.0);
			a3timerStart(demoState->renderTimer);

			// text
			a3demo_initializeText(demoState->text);
			demoState->textInit = 1;
			demoState->textMode = 1;
			demoState->textModeCount = 3;	// 0=off, 1=controls, 2=data

			// peer instance
			if (!rakClient.peer)
			{
				rakClient.peer = RakNet::RakPeerInterface::GetInstance();
				if (rakClient.peer)
				{
					a3DemoNetworking_lobby_init();
				}
			}

			/*
			// enable asset streaming between loads
		//	demoState->streaming = 1;

			// create directory for data
			a3fileStreamMakeDirectory("./data");


			// set default GL state
			a3demo_setDefaultGraphicsState();

			// geometry
			a3demo_loadGeometry(demoState);

			// shaders
			a3demo_loadShaders(demoState);

			// scene objects
			a3demo_initScene(demoState);
			*/
		}
	}

	// return persistent state pointer
	return demoState;
}

// demo is unloaded; option to unload to prep for hotbuild
A3DYLIBSYMBOL a3_DemoState *a3demoCB_unload(a3_DemoState *demoState, a3boolean hotbuild)
{
	// release things that need releasing always, whether hotbuilding or not
	// e.g. kill thread
	// nothing in this example, but then...

	// release persistent state if not hotbuilding
	// good idea to release in reverse order that things were loaded...
	//	...normally; however, in this case there are reference counters 
	//	to make sure things get deleted when they need to, so there is 
	//	no need to reverse!
	if (!hotbuild)
	{
		// free fixed objects
		a3textRelease(demoState->text);

		// unload peer when unloading
		if (rakClient.peer)
		{
			RakNet::RakPeerInterface::DestroyInstance(rakClient.peer);
			rakClient.peer = 0;
		}

		/*
		// free graphics objects
		a3demo_unloadGeometry(demoState);
		a3demo_unloadShaders(demoState);

		// validate unload
		a3demo_validateUnload(demoState);
		*/

		// erase other stuff
		a3trigFree();


		// erase persistent state
		free(demoState);
		demoState = 0;
	}

	// return state pointer
	return demoState;
}

// window updates display
// **NOTE: DO NOT USE FOR RENDERING**
A3DYLIBSYMBOL a3i32 a3demoCB_display(a3_DemoState *demoState)
{
	// do nothing, should just return 1 to indicate that the 
	//	window's display area is controlled by the demo
	return 1;
}

// window idles
A3DYLIBSYMBOL a3i32 a3demoCB_idle(a3_DemoState *demoState)
{
	// perform any idle tasks, such as rendering
	if (!demoState->exitFlag)
	{
		if (a3timerUpdate(demoState->renderTimer) > 0)
		{
			// render timer ticked, update demo state and draw
			//a3demo_update(demoState, demoState->renderTimer->secondsPerTick);
			//a3demo_input(demoState, demoState->renderTimer->secondsPerTick);
			//a3demo_render(demoState);

			// Update input
			a3mouseUpdate(demoState->mouse);
			a3keyboardUpdate(demoState->keyboard);
			a3XboxControlUpdate(demoState->xcontrol);

			// Update
			a3DemoUpdate(demoState);

			// render occurred this idle: return +1
			return +1;
		}

		// nothing happened this idle: return 0
		return 0;
	}

	// demo should exit now: return -1
	return -1;
}

// window gains focus
A3DYLIBSYMBOL void a3demoCB_windowActivate(a3_DemoState *demoState)
{
	// nothing really needs to be done here...
	//	but it's here just in case
}

// window loses focus
A3DYLIBSYMBOL void a3demoCB_windowDeactivate(a3_DemoState *demoState)
{
	// reset input; it won't track events if the window is inactive, 
	//	active controls will freeze and you'll get strange behaviors
	a3keyboardReset(demoState->keyboard);
	a3mouseReset(demoState->mouse);
	a3XboxControlReset(demoState->xcontrol);
	a3XboxControlSetRumble(demoState->xcontrol, 0, 0);
}

// window moves
A3DYLIBSYMBOL void a3demoCB_windowMove(a3_DemoState *demoState, a3i32 newWindowPosX, a3i32 newWindowPosY)
{
	// nothing needed here
}

// window resizes
A3DYLIBSYMBOL void a3demoCB_windowResize(a3_DemoState *demoState, a3i32 newWindowWidth, a3i32 newWindowHeight)
{
	/*
	a3ui32 i;
	a3_DemoCamera *camera;
	*/

	// account for borders here
	const a3i32 frameBorder = 0;
	const a3ui32 frameWidth = newWindowWidth + frameBorder + frameBorder;
	const a3ui32 frameHeight = newWindowHeight + frameBorder + frameBorder;
	const a3real windowAspect = (a3real)newWindowWidth / (a3real)newWindowHeight;
	const a3real frameAspect = (a3real)frameWidth / (a3real)frameHeight;

	// copy new values to demo state
	demoState->windowWidth = newWindowWidth;
	demoState->windowHeight = newWindowHeight;
	demoState->windowWidthInv = a3recip((a3real)newWindowWidth);
	demoState->windowHeightInv = a3recip((a3real)newWindowHeight);
	demoState->windowAspect = windowAspect;
	demoState->frameWidth = frameWidth;
	demoState->frameHeight = frameHeight;
	demoState->frameWidthInv = a3recip((a3real)frameWidth);
	demoState->frameHeightInv = a3recip((a3real)frameHeight);
	demoState->frameAspect = frameAspect;
	demoState->frameBorder = frameBorder;

	// framebuffers should be initialized or re-initialized here 
	//	since they are likely dependent on the window size


	// use framebuffer deactivate utility to set viewport
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable, -frameBorder, -frameBorder, demoState->frameWidth, demoState->frameHeight);

	/*
	// viewing info for projection matrix
	// initialize cameras dependent on viewport
	for (i = 0, camera = demoState->camera + i; i < demoStateMaxCount_cameraObject; ++i, ++camera)
	{
		camera->aspect = frameAspect;
		a3demo_updateCameraProjection(camera);
	}
	*/
}

// any key is pressed
A3DYLIBSYMBOL void a3demoCB_keyPress(a3_DemoState *demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_down);
}

// any key is held
A3DYLIBSYMBOL void a3demoCB_keyHold(a3_DemoState *demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_down);
}

// any key is released
A3DYLIBSYMBOL void a3demoCB_keyRelease(a3_DemoState *demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_up);
}

// ASCII key is pressed (immediately preceded by "any key" pressed call above)
// NOTE: there is no release counterpart
A3DYLIBSYMBOL void a3demoCB_keyCharPress(a3_DemoState *demoState, a3i32 asciiKey)
{
	/*
	a3ui32 demoSubMode = demoState->demoSubMode[demoState->demoMode];
	const a3ui32 demoSubModeCount = demoState->demoSubModeCount[demoState->demoMode];
	const a3ui32 demoOutput = demoState->demoOutputMode[demoState->demoMode][demoSubMode];
	const a3ui32 demoOutputCount = demoState->demoOutputCount[demoState->demoMode][demoSubMode];
	*/

	// persistent state update
	a3keyboardSetStateASCII(demoState->keyboard, (a3byte)asciiKey);

	// switch statement for keyboard input
	// Ascii table for reference: http://www.asciitable.com/
	switch (asciiKey)
	{
	// Backspace
	case 8:
		if (cInput.bufferWriteLoc != 0)
		{
			// Remove previous input value
			cInput.ClearLastValue();
		}
		break;

	// Carriage return (Enter)
	case 13:
		// Make sure line isn't empty
		if (cInput.buffer[0] != 0)
		{
			// Add input buffer to chat buffer
			cChat.In(cInput.buffer);
			// TO-DO Parse input and do whatever
		}

		// empty input buffer
		cInput.ClearChatBuffer();
		break;

	case 27:
		rakClient.connected = false;

	// Remaining input
	default:
		// If there is input, add it to input buffer
		cInput.In(asciiKey);
		break;
	}

	/*
	// handle special cases immediately
	switch (asciiKey)
	{
		// uncomment to make escape key kill the current demo
		// if disabled, use 'exit demo' menu option
//	case 27: 
//		demoState->exitFlag = 1;
//		break;

		// reload (T) or toggle (t) text
	case 'T':
		if (!a3textIsInitialized(demoState->text))
		{
			a3demo_initializeText(demoState->text);
			demoState->textInit = 1;
		}
		else
		{
			a3textRelease(demoState->text);
			demoState->textInit = 0;
		}
		break;
	case 't':
		demoState->textMode = (demoState->textMode + 1) % demoState->textModeCount;
		break;

		// reload all shaders in real-time
	case 'P':
		a3demo_unloadShaders(demoState);
		a3demo_loadShaders(demoState);
		break;


		// change pipeline mode
	case '.':
		demoState->demoMode = (demoState->demoMode + 1) % demoState->demoModeCount;
		break;
	case ',':
		demoState->demoMode = (demoState->demoMode + demoState->demoModeCount - 1) % demoState->demoModeCount;
		break;

		// change pipeline stage
	case '>':
		demoSubMode = demoState->demoSubMode[demoState->demoMode] = (demoSubMode + 1) % demoSubModeCount;
		break;
	case '<':
		demoSubMode = demoState->demoSubMode[demoState->demoMode] = (demoSubMode + demoSubModeCount - 1) % demoSubModeCount;
		break;

		// change stage output
	case '}':
		demoState->demoOutputMode[demoState->demoMode][demoSubMode] = (demoOutput + 1) % demoOutputCount;
		break;
	case '{':
		demoState->demoOutputMode[demoState->demoMode][demoSubMode] = (demoOutput + demoOutputCount - 1) % demoOutputCount;
		break;


		// toggle grid
	case 'g':
		demoState->displayGrid = 1 - demoState->displayGrid;
		break;

		// toggle world axes
	case 'x':
		demoState->displayWorldAxes = 1 - demoState->displayWorldAxes;
		break;

		// toggle object axes
	case 'z':
		demoState->displayObjectAxes = 1 - demoState->displayObjectAxes;
		break;

		// toggle tangent bases on vertices or other
	case 'B':
		demoState->displayTangentBases = 1 - demoState->displayTangentBases;
		break;


		// update animation
	case 'm':
		demoState->updateAnimation = 1 - demoState->updateAnimation;
		break;
	}


	// callback for current mode
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demoCB_keyCharPress_main(demoState, asciiKey,
			demoSubMode, demoOutput, demoSubModeCount, demoOutputCount);
		break;
	}
	*/
}

// ASCII key is held
A3DYLIBSYMBOL void a3demoCB_keyCharHold(a3_DemoState *demoState, a3i32 asciiKey)
{
	// persistent state update
	a3keyboardSetStateASCII(demoState->keyboard, (a3byte)asciiKey);

	// ** Same switch as keyCharPress, but used for when key is held
	// switch statement for keyboard input
	// Ascii table for reference: http://www.asciitable.com/
	switch (asciiKey)
	{
	// Backspace
	case 8:
		if (cInput.bufferWriteLoc != 0)
		{
			// Remove previous input value
			cInput.ClearLastValue();
		}
		break;

	// Remaining input
	default:
		// If there is input, add it to input buffer
		cInput.In(asciiKey);
		break;
	}

	/*
	// callback for current mode
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demoCB_keyCharHold_main(demoState, asciiKey);
		break;
	}
	*/
}

// mouse button is clicked
A3DYLIBSYMBOL void a3demoCB_mouseClick(a3_DemoState *demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->mouse, (a3_MouseButton)button, a3input_down);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse button is double-clicked
A3DYLIBSYMBOL void a3demoCB_mouseDoubleClick(a3_DemoState *demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->mouse, (a3_MouseButton)button, a3input_down);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse button is released
A3DYLIBSYMBOL void a3demoCB_mouseRelease(a3_DemoState *demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->mouse, (a3_MouseButton)button, a3input_up);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse wheel is turned
A3DYLIBSYMBOL void a3demoCB_mouseWheel(a3_DemoState *demoState, a3i32 delta, a3i32 cursorX, a3i32 cursorY)
{
	// controlled camera when zooming
	// a3_DemoCamera* camera;

	// persistent state update
	a3mouseSetStateWheel(demoState->mouse, (a3_MouseWheelState)delta);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);

	// set view location if scrolling
	cChat.bufferViewOffset = max(0, min(cChat.bufferWriteLoc - cChat.CHAT_VIEW_MAX, cChat.bufferViewOffset + (-1 * delta)));

	/*
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		// can use this to change zoom
		// zoom should be faster farther away
		camera = demoState->camera + demoState->activeCamera;
		camera->fovy -= camera->ctrlZoomSpeed * (camera->fovy / a3real_oneeighty) * (a3f32)delta;
		camera->fovy = a3clamp(camera->ctrlZoomSpeed, a3real_oneeighty - camera->ctrlZoomSpeed, camera->fovy);
		a3demo_updateCameraProjection(camera);
		break;
	}
	*/
}

// mouse moves
A3DYLIBSYMBOL void a3demoCB_mouseMove(a3_DemoState *demoState, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse leaves window
A3DYLIBSYMBOL void a3demoCB_mouseLeave(a3_DemoState *demoState)
{
	// reset mouse state or any buttons pressed will freeze
	a3mouseReset(demoState->mouse);
}


//-----------------------------------------------------------------------------
