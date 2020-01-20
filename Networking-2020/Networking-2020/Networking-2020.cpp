#include <stdio.h>
#include <string.h>
#include <sstream>
#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"  // MessageID

// NOTES:
// Command Prompt > ipconfig > IPv4 (gets global IP of computer for server connection)

// number of maximum clients
unsigned int maxClients;
// the server port number
unsigned short serverPort;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	// Custom message identifier for welcoming the player
	ID_GAME_MESSAGE_PLAYER_CONNECTED,
	ID_GAME_MESSAGE_PLAYER_DISCONNECTED
};


// Custom message packet structure /////////
// Force compiler to pack the structure as byte-aligned.
#pragma pack (push, 1)
struct messagePack
{
public: 
	RakNet::MessageID msgID;
	// Should use pre allocated byte array memory instead of string
	std::string msgString;

	// Functions
	messagePack(RakNet::MessageID ID, std::string string) { msgID = ID, msgString = string; };
	void CreatePacket(RakNet::MessageID ID, std::string string) { msgID = ID, msgString = string; };
	
};
#pragma pack (pop)

int main(void)
{
	char str[512];
	// Name of the client 
	char clientName[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;

	// Create our packet variable
	RakNet::Packet *packet;
	// Connected to chat
	bool connected = false;

	
	while (1)
	{
		if (!connected)
		{
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
				// Connected
				connected = true;
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
				// Connected
				connected = true;
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

				// Changed == to 10 since fgets prints enter as 10 instead of 0
				if (str[0] == 10) {
					strcpy(str, "127.0.0.1");
				}
				printf("Starting the client.\n");
				peer->Connect(str, serverPort, 0, 0);

			}
		}
		else
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
				{
					printf("Our connection request has been accepted.\n");

					// Prompt for player's name
					printf("Type your name: ");
					// Read their input and store it
					fgets(clientName, 512, stdin);

					// Create data structure and initialize with our message identifier and message, in this case the user inputted client name
					messagePack pack(ID_GAME_MESSAGE_PLAYER_CONNECTED, clientName);
					// Send the data structure to the server by casting it to a byte stream using const char* and passing the size of our data structure
					peer->Send((const char*)&pack, sizeof(pack), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

					// Prompt for disconnect message (it doesn't actually disconnect yet, just sends a goodbye message)
					printf("Disconnect? Y/N: ");
					// Get input
					fgets(str, 512, stdin);
					// if yes,
					if ((str[0] == 'y') || (str[0] == 'Y'))
					{
						// Create data structure and initialize with our message identifier and message, in this case the user inputted client name
						messagePack pack(ID_GAME_MESSAGE_PLAYER_DISCONNECTED, clientName);
						// Send the data structure to the server by casting it to a byte stream using const char* and passing the size of our data structure
						peer->Send((const char*)&pack, sizeof(pack), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						// Set to disconnect from server after loop is finished
						connected = false;
					}
				}
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
				case ID_GAME_MESSAGE_PLAYER_CONNECTED:
				{
					// Cast packet to our data structure
					messagePack* p = (messagePack*)packet->data;
					// Print the message with the message string from the structure
					printf("Welcome to the server, %s\n", p->msgString.c_str());
				}
				break;

				// Custom message loop for player disconnect
				case ID_GAME_MESSAGE_PLAYER_DISCONNECTED:
				{
					// Cast packet to our data structure
					messagePack* p = (messagePack*)packet->data;
					// Print the message with the message string from the structure
					printf("Goodbye, %s\n", p->msgString.c_str());

				}
				break;

				default:
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
					break;
				}
			}

			// If set to disconnect, shutdown connection between server
			if(!connected)
				peer->Shutdown(50, 0, LOW_PRIORITY);
		}

	}

	// TODO - Add code body here

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}