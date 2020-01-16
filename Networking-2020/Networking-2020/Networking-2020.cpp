#include <stdio.h>
#include "RakNet/RakPeerInterface.h"


// number of maximum clients
unsigned int maxClients;
// the server port number
unsigned short serverPort;



int main(void)
{
	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;

	printf("(C) or (S)erver?\n");
	fgets(str, 512, stdin);
	

	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;

		// Prompt for server port input
		printf("Enter server port number\n");
		// Read user input
		fgets(str, 512, stdin);
		// Set server port to inputed value in str
		serverPort = strtol(str, NULL, 0);
	}
	else {
		
		// Prompt for client number input; only when using a server
		printf("Enter maximum client number\n");
		// Read user input
		fgets(str, 512, stdin);
		// Set max clients to inputed value in str
		maxClients = strtol(str, NULL, 0);

		// Prompt for server port input
		printf("Enter server port number\n");
		// Read user input
		fgets(str, 512, stdin);
		// Set server port to inputed value in str
		serverPort = strtol(str, NULL, 0);
		
		RakNet::SocketDescriptor sd(serverPort, 0);
		peer->Startup(maxClients, &sd, 1);
		isServer = true;
	}

	// TODO - Add code body here

	
	

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}