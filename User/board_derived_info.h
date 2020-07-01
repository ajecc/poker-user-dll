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


std::vector<const card_t*> 
find_remaining_cards(const hand_t* hero, const hand_t* villain, const board_t* board);


board_derived_info_t
get_board_derived_info(player_t* hero, board_t* board);
