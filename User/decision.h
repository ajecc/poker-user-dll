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
	double pot;
	double current_bet;

	
	// NOTE: this can be expanded nicely 
};


struct decision_t
{
	hand_action_t action;
	double sum;
};


board_derived_info_t get_board_derived_info(player_t* hero, board_t* board);

decision_t take_decision_preflop(player_t* hero, board_t* board);

decision_t take_decision_flop(player_t* hero, board_t* board);

decision_t take_decision_turn(player_t* hero, board_t* board);

decision_t take_decision_river(player_t* hero, board_t* board);

decision_t take_decision(player_t* hero, board_t* board);
