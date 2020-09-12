#pragma once
#include "hand.h"


enum bet_type_t
{
	INVALID_BET_TYPE = -1,
	OPEN = 0,
	FACING_RAISE = 1,
	FACING_3BET = 2,
	FACING_4BET = 3,
	BET_TYPE_COUNT = 4
};


bet_type_t& operator++(bet_type_t& bet_type);


struct board_derived_info_t
{
	bet_type_t bet_type = INVALID_BET_TYPE;
	std::vector<player_t*> villains_before_hero;
	std::vector<player_t*> villains_after_hero;
	player_t* main_villain = nullptr;
	player_t* secondary_villain = nullptr;
	float current_bet = 0;
	float call_ammount = 0;
	std::vector<const card_t*> villain_draws_flop;
	std::vector<const card_t*> villain_draws_turn;
	std::vector<const card_t*> hero_draws_flop;
	std::vector<const card_t*> hero_draws_turn;

	std::string
	to_string();
};


/*
Finds the remaining cards, excluding the ones held by the hero,
a villain and the ones found on the board

Parameters: IN hand_t* - the hand held by the hero. can be nullptr
			IN hand_t* - the hand held by the villain. can be nullptr
			IN board_t* - the board. CAN NOT be nullptr

Returns: std::vector<const card_t*> - the remaining cards 
*/
std::vector<const card_t*> 
find_remaining_cards(const hand_t* hero, const hand_t* villain, const board_t* board);


/*
Fills a board_derived_info_t with the appropriate information 
and returns it.

Parameters: player_t* - the hero
			board_t* - the board

Returns: board_derived_info_t - the appropriate information derived from the board
*/
board_derived_info_t
get_board_derived_info(player_t* hero, board_t* board);
