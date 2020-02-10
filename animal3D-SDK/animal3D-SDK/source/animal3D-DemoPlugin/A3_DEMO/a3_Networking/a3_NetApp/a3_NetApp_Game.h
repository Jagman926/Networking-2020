#ifndef A3_NETAPP_GAME_H
#define A3_NETAPP_GAME_H



enum GameType
{
	NOGAME = 0,
	TICTACTOE,
	CHECKERS
	
};

struct Game
{
public: 
	// Type of game to be played
	GameType type;

	// Names of the players in the game
	char players[10][512];

	// Index of who's turn it is; 0 = Challenger 1; 1 = Challenger 2
	int turnIndex;
	
	// number of spectators in the game
	int numSpectators = 0;

	// checks if a game is in progress
	bool inProgress = false;

	// is the game local or not
	bool isLocal;

	// checks ready ups from both players
	bool ready[2];

	// Game of tictactoe
	//gs_tictactoe* ticTacToeGame;

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

	
};

#endif // !A3_NETAPP_GAME_H

