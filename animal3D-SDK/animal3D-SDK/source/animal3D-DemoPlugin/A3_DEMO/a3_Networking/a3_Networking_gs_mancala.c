/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein

	a3_Networking_gs_mancala.c/.cpp
	Game code for networked Mancala.

	********************************************
	*** Mancala implementation				 ***
	********************************************
*/

#include <stdio.h>


//-----------------------------------------------------------------------------
// DECLARATIONS

#define GS_MANCALA_PLAYERS					2
#define GS_MANCALA_BOARD_HEIGHT				8

enum gs_mancala_space_index
{
	// playable spaces on one player's side
	gs_mancala_space_mancala,	// goal space ("mancala")
	gs_mancala_space_1,			// first space next to mancala
	gs_mancala_space_2,			// second space...
	gs_mancala_space_3,			// third...
	gs_mancala_space_4,			// fourth...
	gs_mancala_space_5,			// fifth...
	gs_mancala_space_6,			// sixth...

	// not a playable space but useful for storing running on-side total
	gs_mancala_total,
};
#ifndef __cplusplus
typedef		enum gs_mancala_space_index		gs_mancala_space_index;
#endif	// !__cplusplus

// descriptor for space
typedef		char							gs_mancala_space_state;
extern		gs_mancala_space_state const	gs_mancala_space_invalid;

// Mancala game state
typedef		unsigned char					gs_mancala_index;
typedef		gs_mancala_space_state			gs_mancala[GS_MANCALA_PLAYERS][GS_MANCALA_BOARD_HEIGHT];


#define	GS_VALIDATE_COORDINATE(x,y,w,h)			(x < w && y < h)
#define GS_MANCALA_VALID(p,s)					GS_VALIDATE_COORDINATE(p,s,GS_MANCALA_PLAYERS,GS_MANCALA_BOARD_HEIGHT)


inline gs_mancala_space_state gs_tictactoe_getSpaceState(gs_mancala const game, gs_mancala_index const player, gs_mancala_index const space)
{
	if (GS_MANCALA_VALID(player, space))
		return (game[player][space]);
	return gs_mancala_space_invalid;
}

inline gs_mancala_space_state gs_tictactoe_setSpaceState(gs_mancala game, gs_mancala_space_state const state, gs_mancala_index const player, gs_mancala_index const space)
{
	if (GS_MANCALA_VALID(player, space))
		return (game[player][space] = state);
	return gs_mancala_space_invalid;
}

inline gs_mancala_index gs_mancala_reset(gs_mancala game)
{
	gs_mancala_index player, space, total;
	for (player = 0; player < GS_MANCALA_PLAYERS; ++player)
	{
		space = gs_mancala_space_mancala;
		game[player][space++] = 0;
		while (space < gs_mancala_total)
			game[player][space++] = 4;
		game[player][space++] = 24;
	}
	total = (player * space);
	return total;
}


//-----------------------------------------------------------------------------
// DEFINITIONS

extern		gs_mancala_space_state const	gs_mancala_space_invalid = -1;


int launchMancala()
{
	gs_mancala game = { 0 };

	gs_mancala_reset(game);



	return 0;
}


//-----------------------------------------------------------------------------
