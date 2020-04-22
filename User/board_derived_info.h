#pragma once
#include "hand.h"
enum bet_type_t
{
	OPEN = 0,
	FACING_RAISE = 1,
	FACING_3BET = 2,
	FACING_4BET = 3
};


bet_type_t& operator++(bet_type_t& bet_type);


struct board_derived_info_t
{
	bet_type_t bet_type;
	std::vector<player_t*> villains_before_hero;
	std::vector<player_t*> villains_after_hero;
	player_t* main_villain;
	player_t* secondary_villain;
	float pot;
	float current_bet;
	std::vector<const card_t*> villain_draws_flop;
	std::vector<const card_t*> villain_draws_turn;
};


std::vector<const card_t*> find_remaining_cards(const hand_t* hero, const hand_t* villain, board_t* board);

board_derived_info_t get_board_derived_info(player_t* hero, board_t* board);
