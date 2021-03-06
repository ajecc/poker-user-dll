#pragma once

#include <vector>
#include "player.h"
#include "card.h"


struct decision_t;
struct board_derived_info_t;


struct board_t
{
	board_stage_t stage = INVALID_BOARD_STAGE;
	/*
	The cards found on the board, in ascending order. (using card_ptr_cmp)
	*/
	std::vector<const card_t*> cards;
	std::vector<player_t*> players;
	/*
	Players in current_hand_players are ordered in according to their position.
	Preflop, UTG would be the first and BB the last, flop+ SB first and BTN last.
	*/
	std::vector<player_t*> current_hand_players;
	player_t* hero = nullptr;
	float pot = 0;
	float big_blind_sum = 0;
	board_derived_info_t* board_derived_info = nullptr;
	decision_t* last_decision = nullptr;

	std::string
	to_string() const;
};


/*
Gets the player by its label, from a board. The label is defined in player_t struct.

Parameters: IN board_t* board -- the board to fetch the player from
			IN string label -- the label of the player

Returns: player_t* -- a pointer to the player fetched. This pointer should not be freed. 
*/
player_t*
get_player_by_label(board_t* board, const std::string& label);


/*
Updated the board. Should be called each tick.

Parameters: OUT board_t* -- the board to update

Returns: none
*/
void
update_board(board_t* board);


/*
Gets the player with lexicographically greater label than the given player.

Parameters: IN board_t* board -- the board to fetch the next player from
			IN player_t* player -- the given player

Returns: player_t* -- the next player after the given player, with lexicographically 
						bigger label.
*/
player_t*
get_next_player(board_t* board, player_t* player);


/*
Gets the next that is currently in game.

Parameters: IN board_t* board -- the board to fetch the next player from
			IN player_t* player -- the given player

Returns: player_t* -- the next player after the given player 
*/
player_t*
get_next_player_in_game(board_t* board, player_t* player);


/*
Gets the next that is currently in hand.

Parameters: IN board_t* board -- the board to fetch the next player from
			IN player_t* player -- the given player

Returns: player_t* -- the next player after the given player 
*/
player_t*
get_next_player_in_hand(board_t* board, player_t* player);


/*
Checks if the board has a wet flop.

Parameters: IN board_t* board - the board to be checked for a wet flop

Returns: bool - true if the board has a wet flop and false otherwise
*/
bool
is_board_wet_flop(const board_t* board);


/*
Creates the main board. This is a global object that should get updated each tick.  
This object can be accessed directly.

Parametrs: none

Returns: board_t*
*/
board_t*
create_board();


/*
Destroys the board created with create_board.

Parameters: OUT board_t** board -- the board to destroy
*/
void
destroy_board(board_t** board);
