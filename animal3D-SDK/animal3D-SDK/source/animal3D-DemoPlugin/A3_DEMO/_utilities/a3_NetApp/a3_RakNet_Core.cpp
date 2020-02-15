#include "a3_RakNet_Core.h"
#include "A3_DEMO/a3_Networking/a3_NetApp/a3_NetApp_Client.h"


User::User(char name[], const char* sysAddress, UserType userType)
{
	std::strcpy(userName, name);
	std::strcpy(systemAddress, sysAddress);
	type = userType;
}