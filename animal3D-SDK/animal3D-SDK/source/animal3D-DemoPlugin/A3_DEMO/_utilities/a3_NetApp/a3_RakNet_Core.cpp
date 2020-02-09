#include "a3_RakNet_Core.h"


User::User(char name[], char sysAddress[], UserType userType)
{
	std::strcpy(userName, name);
	std::strcpy(systemAddress, sysAddress);
	type = userType;

}


void RakClient::NetUpdate(bool connected)
{
	// Initialize peer instance at start
	peer = RakNet::RakPeerInterface::GetInstance();

	while (1)
	{
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
				//User player(inputedName, peer->GetSystemAddressFromGuid(peer->GetMyGUID()), PLAYER);
				
				// init the server port
				serverPort = 1;

				// We're now connected
				connected = true;
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
				//User server(adminName, peer->GetSystemAddressFromGuid(peer->GetMyGUID()).ToString(), SERVER);

			}
		}
	}
}


