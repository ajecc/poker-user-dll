#pragma once
#include "hand.h"


struct decision_t
{
	hand_action_t action;
	float sum;

	std::string to_string();
};


decision_t take_decision_preflop(player_t* hero, board_t* board);

decision_t take_decision_flop(player_t* hero, board_t* board);

decision_t take_decision_turn(player_t* hero, board_t* board);

decision_t take_decision_river(player_t* hero, board_t* board);

decision_t take_decision(player_t* hero, board_t* board);
