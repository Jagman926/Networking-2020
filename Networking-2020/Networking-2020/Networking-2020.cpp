#include <stdio.h>
#include "RakNet/RakPeerInterface.h"
#include <string.h>
#include "RakNet/MessageIdentifiers.h"

// number of maximum clients
unsigned int maxClients;
// the server port number
unsigned short serverPort;



int main(void)
{
	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;

	// Create our packet variable
	RakNet::Packet* packet;

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

	if (isServer)
	{
		printf("Starting the server!!!\n");
		// We need to let the server accept incoming connections from the clients
		peer->SetMaximumIncomingConnections(maxClients);
	}
	else 
	{
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 512, stdin);
		if (str[0] == 0) {
			strcpy(str, "127.0.0.1");
		}
		printf("Starting the client.\n");
		peer->Connect(str, serverPort, 0, 0);

	}
	
	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
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
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}

	// TODO - Add code body here

	
	

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}