#include "a3_RakNet_Core.h"
#include "A3_DEMO/a3_Networking/a3_NetApp/a3_NetApp_Client.h"


User::User(char name[], const char* sysAddress, UserType userType)
{
	std::strcpy(userName, name);
	std::strcpy(systemAddress, sysAddress);
	type = userType;

}

	/* ------------------------------------------------------------------------------------------------------- */
	/*                                            USER INPUT COMMANDS AND MESSAGES                             */
	/* ------------------------------------------------------------------------------------------------------- */

	// DAN
	// get input	
	//msg = "";
	//printf("Input command: ");
	//// TODO: switch to fgets, make msg a char[]
	////std::cin >> msg;
	//
	//// if command is being input; 47 = '/'
	//if (msg.at(0) == '/')
	//{
	//	// string to hold our command that we will split up
	//	std::string command;
	//
	//	// gets string from after / to first space
	//	command = msg.substr(1, msg.find(' '));
	//
	//	/* ------------------------------------------------------------------------------------------------------- */
	//	/*                                            SERVER ONLY COMMANDS                                         */
	//	/* ------------------------------------------------------------------------------------------------------- */
	//
	//	// Prints out user name and IP of all connected users to host console, including host
	//	if (thisUser.type == SERVER && command == "users")
	//	{
	//		for (int i = 0; i < currentUsers; i++)
	//		{
	//			//printf("User: %s | Address: %s\n", users[i].userName, users[i].systemAddress.c_str());
	//		}
	//	}
	//
	//	/* ------------------------------------------------------------------------------------------------------- */
	//	/*                                            USER COMMANDS                                                */
	//	/* ------------------------------------------------------------------------------------------------------- */
	//
	//	// Outputs all messaging/command types
	//	else if (command == "help")
	//	{
	//		printf("Commands\n/disconnect - Disconnect from the server\n/msg all: - Broadcast message to all users\n/msg [name]: - Sends a private message to the specified user\n");
	//		if (thisUser.type == SERVER)
	//		{
	//			printf("\nAdmin Commands\n/ users - Display all usernames & IP\n");
	//		}
	//
	//	}
	//	else if (command == "msg")
	//	{
	//		// string to hold the user name to send to
	//		char user[512];
	//		char message[512];
	//		strcpy(user, msg.c_str());
	//		char* parse;
	//		// gets string from space to : 
	//		// TODO: Figure out correct parsing
	//		parse = strtok(user, "");
	//		parse = strtok(parse, " ");
	//
	//		// Create our private message request
	//		ChatMessageRequest messageRequest(ID_CHAT_MSG_REQUEST, user, message);
	//		// Send to host
	//		peer->Send((const char*)&messageRequest, sizeof(messageRequest), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	//	}
	//
	//	else if (command == "all")
	//	{
	//		// string to hold the user name to send to
	//		std::string  message;
	//
	//		message = msg.substr(msg.find("all") + 1);
	//
	//		// Create our private message request
	//		ChatMessageRequest messageRequest(ID_CHAT_MSG_REQUEST, "all", message.c_str());
	//		// Send to host
	//		//peer->Send((const char*)&messageRequest, sizeof(messageRequest), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::AddressOrGUID)hostSystemAddress.c_str(), false);
	//	}
	//	else if (command == "disconnect")
	//	{
	//		// Prompt for disconnect message
	//		printf("Are you sure? Y/N: ");
	//		// Get input
	//		fgets(str, 512, stdin);
	//		// if yes,
	//		if ((str[0] == 'y') || (str[0] == 'Y'))
	//		{
	//			// Create data structure and initialize with our message identifier and message, in this case the user inputted client name
	//			messagePack pack(ID_GAME_MSG_PLAYER_DISCONNECTED, thisUser.userName);
	//			// Send the data structure to the server by casting it to a byte stream using const char* and passing the size of our data structure
	//			peer->Send((const char*)&pack, sizeof(pack), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	//			// Set to disconnect from server after loop is finished
	//			connected = false;
	//		}
	//	}
	//	else
	//	{
	//		printf("%s is an invalid command! Type /help to see list of all commands\n", command.c_str());
	//	}
	//
	//}


