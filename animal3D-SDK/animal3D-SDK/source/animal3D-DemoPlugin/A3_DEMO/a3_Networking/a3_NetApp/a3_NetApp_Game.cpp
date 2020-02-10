#include "A3_DEMO/a3_Networking/a3_NetApp/a3_NetApp_Game.h"
#include <string>



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
	if (strcmp(player, players[0]) == 0)
		return true;
	else if (strcmp(player, players[1]) == 0)
		return true;
	else
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
	if (strcmp(player, players[1]) == 0)
	{
		ready[1] = isReady;
	}
}

void TicTacToeBoard::SetSpace(int num, TicTacToeSpace spaceState)
{
	int x = (int)(num / 3);
	int y = num % 3;

	SetSpace(x, y, spaceState);

}

TicTacToeSpace TicTacToeBoard::GetPlayerSpaceType()
{
	if (turnIndex == 0)
	{
		return TicTacToeSpace::TICTACTOE_X;
	}
	else if (turnIndex == 1)
	{
		return TicTacToeSpace::TICTACTOE_O;
	}
	return TicTacToeSpace::EMPTY;
}

TicTacToeSpace TicTacToeBoard::GetPlayerOtherSpaceType()
{
	if (turnIndex == 0)
	{
		return TicTacToeSpace::TICTACTOE_O;
	}
	else if (turnIndex == 1)
	{
		return TicTacToeSpace::TICTACTOE_X;
	}
	return TicTacToeSpace::EMPTY;
}

void TicTacToeBoard::NextTurn()
{
	if (turnIndex == 0)
	{
		turnIndex = 1;
	}
	else if (turnIndex == 1)
	{
		turnIndex = 0;
	}
}
