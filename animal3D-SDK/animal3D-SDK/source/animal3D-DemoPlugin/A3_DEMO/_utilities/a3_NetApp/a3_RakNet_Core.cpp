#include "a3_RakNet_Core.h"


User::User(char name[], const char* sysAddress, UserType userType)
{
	std::strcpy(userName, name);
	std::strcpy(systemAddress, sysAddress);
	type = userType;

}


void RakClient::NetUpdate(bool connected)
{
	// Initialize peer instance at start
	peer = RakNet::RakPeerInterface::GetInstance();
	/* ----------------------- VARIABLES ----------------------------- */
	if (!connected)
	{
		// TO-DO: Prompt user for inputting their user name
		// TO-DO: Ask user if they want to join or host a game room
		char inputedName[512] = "A";

		// Join as Player
		if (true)
		{
			// Start the Peer
			RakNet::SocketDescriptor sd;
			peer->Startup(1, &sd, 1);

			// Create the user for the player
			User player(inputedName, peer->GetSystemAddressFromGuid(peer->GetMyGUID()).ToString(), PLAYER);

			// init the server port
			serverPort = 1;

			// We're now connected
			connected = true;

			// Set this rakclient to be a player
			thisUser = player;
		}
		// Host a Game Room
		else if (false)
		{
			// init the server port
			serverPort = 1;

			// Start the Peer
			RakNet::SocketDescriptor sd(serverPort, 0);
			peer->Startup(maxClients, &sd, 1);

			// create the server's name which will always be the same so its recognizable
			char adminName[512] = "Admin";

			// Create the User representing the server
			User server(adminName, peer->GetSystemAddressFromGuid(peer->GetMyGUID()).ToString(), SERVER);

			// Add the server to the user list
			users[currentUsers] = server;

			// increment current users now that we have one
			currentUsers++;

			// Connected
			connected = true;

			// Set this rakclient to be a server
			thisUser = server;
		}
		else
		{
			// Disconnect
			connected = false;
			break;
		}
		// Server initialization
		if (thisUser.type == SERVER)
		{
			// TO-DO: Print "Starting the Server!"

			// We need to let the server accept incoming connections from the clients
			peer->SetMaximumIncomingConnections(maxClients);


		}
		// Player initialization
		else
		{
			// TO-DO: Print "Enter server IP or hit enter for 127.0.0.1\n"
			// TO-DO: Get user input
			// TO-DO: Check if presed enter and copy "127.0.0.1");
			// TO-DO: Print "Starting Client!"
			//hostSystemAddress = "184.171.150.100|1";
			// TO-DO: Connect with the host value input by user
			//peer->Connect(INPUT_VALUE, serverPort, 0, 0);
		}
	}
	else
	{
	}
}

char* RakClient::PacketHandling(bool connected)
{
	// For loop for reading packet information
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
			/* ------------------------------------------------------------------------------------------------------- */
			/*                                            DEFAULT MESSAGES                                             */
			/* ------------------------------------------------------------------------------------------------------- */

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
		{
			printf("Our connection request has been accepted.\n");
			// Prompt for player's name
			printf("Type your name: ");
			// Read their input and store it
			fgets(thisUser.userName, 512, stdin);
			// Gets the index of any new line or carriage return and removes it from the char array
			thisUser.userName[strcspn(thisUser.userName, "\r\n")] = 0;

			// Create data structure and initialize with our message identifier and message, in this case the user inputted client name
			messagePack pack(ID_GAME_MSG_PLAYER_CONNECTED, thisUser.userName);
			// Send the data structure to the server by casting it to a byte stream using const char* and passing the size of our data structure
			peer->Send((const char*)&pack, sizeof(pack), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (thisUser.type == SERVER) {
				printf("A client has disconnected.\n");
			}
			else {
				printf("We have been disconnected.\n");
			}
			break;
		case ID_CONNECTION_LOST:
			if (thisUser.type == SERVER) {
				printf("A client lost the connection.\n");
			}
			else {
				printf("Connection lost.\n");
			}
			break;

			/* ------------------------------------------------------------------------------------------------------- */
			/*                                            CUSTOM MESSAGES                                              */
			/* ------------------------------------------------------------------------------------------------------- */

		case ID_GAME_MESSAGE_1:
		{
			RakNet::RakString rs;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rs);
			printf("%s\n", rs.C_String());
		}
		break;

		// Custom message loop for player welcome
		case ID_GAME_MSG_PLAYER_CONNECTED:
		{
			// Cast packet to our data structure
			messagePack* p = (messagePack*)packet->data;

			// Create the client
			User client(p->msgText, packet->systemAddress.ToString());

			// Add the client to the user list
			users[currentUsers] = client;

			// increment current users
			currentUsers++;

			// Print the message with the message string from the structure
			printf("%s has joined the chat!\n", p->msgText);

		}
		break;

		// Custom message loop for player disconnect
		case ID_GAME_MSG_PLAYER_DISCONNECTED:
		{
			// Cast packet to our data structure
			messagePack* p = (messagePack*)packet->data;
			// Print the message with the message string from the structure
			printf("Goodbye, %s\n", p->msgText);
		}
		break;
		// DAN
		case ID_CHAT_MSG_REQUEST:
		{
			// cast packet to data
			ChatMessageRequest* p = (ChatMessageRequest*)packet->data;

			// Name to designate in the message delivery
			char senderName[512];
			// control if we broadcast
			bool isBroadcast = false;
			// read through user database
			for (int i = 0; i < currentUsers; i++)
			{
				// check the system address against the one in the packet
				if (users[i].systemAddress == packet->systemAddress.ToString())
				{
					// copy it on over
					strcpy(senderName, users[i].userName);
				}
			}

			// check if we broadcastin
			if (p->recieverUserName == "all")
				isBroadcast = true;

			// build message delivery data
			ChatMessageDelivery msgDelivery(ID_CHAT_MSG_DELIVERY, senderName, isBroadcast, p->msgTxt);
			peer->Send((const char*)&msgDelivery, sizeof(msgDelivery), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, isBroadcast);
		}
		break;
		// DAN
		case ID_CHAT_MSG_DELIVERY:
		{
			// cast packet to data
			ChatMessageDelivery* p = (ChatMessageDelivery*)packet->data;
			if (p->priv)
			{
				printf("(Private) %s : %s", p->senderUserName, p->msgTxt);
			}
			else
			{
				printf("%s : %s", p->senderUserName, p->msgTxt);
			}
		}
		break;

		case ID_CHAT_MSG_BROADCAST:
		{
			// Cast packet to our data structure
			messagePack* p = (messagePack*)packet->data;
			// Print the message with the message string from the structure
			printf(p->msgText);
		}
		break;

		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}


	/* ------------------------------------------------------------------------------------------------------- */
	/*                                            USER INPUT COMMANDS AND MESSAGES                             */
	/* ------------------------------------------------------------------------------------------------------- */

	// DAN
	// get input	
	msg = "";
	printf("Input command: ");
	// TODO: switch to fgets, make msg a char[]
	std::cin >> msg;

	// if command is being input; 47 = '/'
	if (msg.at(0) == '/')
	{
		// string to hold our command that we will split up
		std::string command;

		// gets string from after / to first space
		command = msg.substr(1, msg.find(' '));

		/* ------------------------------------------------------------------------------------------------------- */
		/*                                            SERVER ONLY COMMANDS                                         */
		/* ------------------------------------------------------------------------------------------------------- */

		// Prints out user name and IP of all connected users to host console, including host
		if (isServer && command == "users")
		{
			for (int i = 0; i < currentUsers; i++)
			{
				printf("User: %s | Address: %s\n", users[i].userName, users[i].systemAddress.c_str());
			}
		}

		/* ------------------------------------------------------------------------------------------------------- */
		/*                                            USER COMMANDS                                                */
		/* ------------------------------------------------------------------------------------------------------- */

		// Outputs all messaging/command types
		else if (command == "help")
		{
			printf("Commands\n/disconnect - Disconnect from the server\n/msg all: - Broadcast message to all users\n/msg [name]: - Sends a private message to the specified user\n");
			if (isServer)
			{
				printf("\nAdmin Commands\n/ users - Display all usernames & IP\n");
			}

		}
		else if (command == "msg")
		{
			// string to hold the user name to send to
			char user[512];
			char message[512];
			strcpy(user, msg.c_str());
			char* parse;
			// gets string from space to : 
			// TODO: Figure out correct parsing
			parse = strtok(user, "");
			parse = strtok(parse, " ");

			// Create our private message request
			ChatMessageRequest messageRequest(ID_CHAT_MSG_REQUEST, user, message);
			// Send to host
			peer->Send((const char*)&messageRequest, sizeof(messageRequest), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		}

		else if (command == "all")
		{
			// string to hold the user name to send to
			std::string  message;

			message = msg.substr(msg.find("all") + 1);

			// Create our private message request
			ChatMessageRequest messageRequest(ID_CHAT_MSG_REQUEST, "all", message.c_str());
			// Send to host
			peer->Send((const char*)&messageRequest, sizeof(messageRequest), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)hostSystemAddress.c_str(), false);
		}
		else if (command == "disconnect")
		{
			// Prompt for disconnect message
			printf("Are you sure? Y/N: ");
			// Get input
			fgets(str, 512, stdin);
			// if yes,
			if ((str[0] == 'y') || (str[0] == 'Y'))
			{
				// Create data structure and initialize with our message identifier and message, in this case the user inputted client name
				messagePack pack(ID_GAME_MSG_PLAYER_DISCONNECTED, clientName);
				// Send the data structure to the server by casting it to a byte stream using const char* and passing the size of our data structure
				peer->Send((const char*)&pack, sizeof(pack), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				// Set to disconnect from server after loop is finished
				connected = false;
			}
		}
		else
		{
			printf("%s is an invalid command! Type /help to see list of all commands\n", command.c_str());
		}

	}
	// If set to disconnect, shutdown connection between server
	if (!connected)
		peer->Shutdown(50, 0, LOW_PRIORITY);

	return nullptr;
}


