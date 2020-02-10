#include "A3_DEMO/a3_Networking/a3_NetApp/a3_NetApp_Game.h"
#include <string>

// Game Includes
#include "..\..\source\animal3D-DemoPlugin\A3_DEMO\a3_Networking\a3_Networking_gs_tictactoe.c"
#include "..\..\source\animal3D-DemoPlugin\A3_DEMO\a3_Networking\a3_Networking_gs_checkers.c"

Game::Game(char player1[512], char player2[512], bool local, GameType gameType)
{
	
	strcpy(players[0], player1);
	strcpy(players[1], player2);

	ready[0] = false;
	ready[1] = false;

	type = gameType;

	isLocal = local;

}

bool Game::IsPlayer(char player[512])
{
	if (strcmp(player, players[0]) || strcmp(player, players[1]))
		return true;
	return false;
	
}

void Game::SetToSpectator(char name[512])
{
	// copy the name of the spectator to the array
	strcpy(players[2 + numSpectators], name);
	// increase number of spectators we have
	numSpectators++;
}

void Game::PlayerReady(char player[512], bool isReady)
{
	if (strcmp(player, players[0]) == 0)
	{
		ready[0] = isReady;
	} 

	else if (strcmp(player, players[1]) == 0)
	{
		ready[1] = isReady;
	}
}
