#pragma once
#include "hand.h"


struct decision_t
{
	hand_action_t action = INVALID_HAND_ACTION;
	float sum = 0;

	std::string
	to_string();
};


/*
Takes the decision preflop. To be used in take_decision.
Also updates some info specific to taking a decision
for both the board and the hero

Parameters: OUT player_t* - the hero
			OUT board_t* - the board

Returns: decisition_t - the decision
*/
decision_t 
take_decision_preflop(player_t* hero, board_t* board);


/*
Takes the decision for flop, turn and river. To be used in take_decision.
Also updates some info specific to taking a decision
for both the board and the hero

Parameters: OUT player_t* - the hero
			OUT board_t* - the board

Returns: decisition_t - the decision
*/
decision_t
take_decision_postflop(player_t* hero, board_t* board);


/*
Takes the decision for all stages of the game.
Also updates some info specific to taking a decision
for both the board and the hero.

Parameters: OUT player_t* - the hero
			OUT board_t* - the board

Returns: decisition_t - the decision
*/
decision_t
take_decision(player_t* hero, board_t* board);
