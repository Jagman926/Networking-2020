#ifndef A3_NETAPP_GAME_H
#define A3_NETAPP_GAME_H

#include <string>

enum GameType
{
	NOGAME = 0,
	TICTACTOE,
	CHECKERS
	
};

enum TicTacToeSpace
{
	EMPTY = 0,
	TICTACTOE_X,
	TICTACTOE_O
};

struct TicTacToeBoard
{
	TicTacToeBoard() { ResetBoard(); };

	// Index of who's turn it is; 0 = Challenger 1; 1 = Challenger 2
	int turnIndex;
	
	// state of the board
	TicTacToeSpace boardState[3][3];

	void ResetBoard() {std::memset(boardState, 0, sizeof(boardState[0][0]) * 3 * 3);};

	void SetSpace(int x, int y, TicTacToeSpace spaceState) { boardState[x][y] = spaceState; };

	void SetSpace(int num, TicTacToeSpace spaceState);

	TicTacToeSpace GetSpaceState(int x, int y) { return boardState[x][y]; };

	TicTacToeSpace GetPlayerSpaceType();
	TicTacToeSpace GetPlayerOtherSpaceType();

	void NextTurn();

};


struct Game
{
public: 
	// Type of game to be played
	GameType type;

	// Names of the players in the game
	char players[10][512];

	// number of spectators in the game
	int numSpectators = 0;

	// checks if a game is in progress
	bool inProgress = false;

	// is the game local or not
	bool isLocal;

	// checks ready ups from both players
	bool ready[2];

	// -------------------FUNCTIONS----------------//
	
	// default ctor
	Game() {};

	// player ctor
	Game(char player1[512], char player2[512], bool local, GameType gameType);

	// check if user is a player
	bool IsPlayer(char player[512]);

	// puts non player in as spectator
	void SetToSpectator(char name[512]);

	// Sets the ready up states for the players
	void PlayerReady(char player[512], bool isReady);

	// TicTacToe
	TicTacToeBoard ticTacToe;

};

#endif // !A3_NETAPP_GAME_H

