#pragma once
#include <vector>
#include "hand.h"
#include "prwin_calc.h"
#include "board_derived_info.h"


#define ALL_PRWIN_VS_ANY_HAND_FLOP_SHARED_NAME "ALL_PRWIN_VS_ANY_HAND_FLOP_SHARED"
#define ALL_PRWIN_VS_ANY_HAND_FLOP_FILE_NAME "all_prwin_vs_any_hand_flop.cache"


/*
Given a hero's hand, a villain's hand and a board, checks if the hero is the winner.

Parameters: IN hand_t* hero -- hero's hand 
			IN hand_t* villain -- villain's hand
			IN board_t* board -- the board. Onlt board->cards is relevant.

Returns: bool -- true if the hero is the winner and false otherwise.
				In case of draw, the hero is also considered a winner.
*/
bool 
is_hero_winner(const hand_t* hero, const hand_t* villain, const board_t* board);


/*
Given a hero's hand, a villain's hand and a board, calculates the chance that the
hero will beat the villain. In case of draw, the hero is considered the winner.

Parameters: IN hand_t* hero -- hero's hand 
			IN hand_t* villain -- villain's hand
			IN board_t* board -- the board. Onlt board->cards is relevant.

Returns: float -- the chance that the hero wins. Value is between [0, 1].
*/
float
calc_prwin_vs_hand(const hand_t* hero, const hand_t* villain, const board_t* board);


/*
Given a hero's hand and a board, calculates the chances that it will win vs any hand.
The function only covers 1v1 scenarios.
NOTE: can be called async

Parameters: IN hand_t* hero -- hero's hand 
			IN board_t* board -- the board. Only board->cards is relevant.

Returns: float -- the chance that the hero wins. Value is between [0, 1].
*/
float 
calc_prwin_vs_any_hand(const hand_t* hero, const board_t* board);


/*
Given a hero's hand and a board, along with some draws, this function
calculates the probability that the hero will win if it hits one of these draws.
The board stage should be FLOP or TURN

Parameters: IN hand_t* hero -- hero's hand
			IN board_t* board -- the board. Only board->cards is relevant.
			IN std::vector<card_t*> draws -- hero's draws

Returns: float -- the chance that the hero wins after hitting a draw.
					Value is between [0, 1].
*/
float
calc_prwin_vs_any_hand_with_draws(const hand_t* hero, const board_t* board,
									const std::vector<const card_t*>& draws);

/*
Calculates the probability for the hero to win vs
a villain's range, for a certain board

Parameters: IN hand_t* - hero's hand
			IN villain_range_t* - villain's range
			IN board_t* - the board

Returns: float - the probability that the hero beats a villain's range
*/
float
calc_prwin_vs_villain_range(const hand_t* hero, const villain_range_t* villain_range, const board_t* board);

/*
Calculates the rate for which the villain would call a hero
bet, for a certain range. The calling rate is normalized,
using the probability for the hero to win vs a hand.

Parameters: IN hand_t* - hero's hand
			IN villain_range_t* - villain's range
			IN board_t* - the board
			IN bet_type_t - the bet type the hero induces

Returns: float - the calling rate
*/
float
calc_calling_rate_vs_villain_range(const hand_t* hero, const villain_range_t* villain_range,
	const board_t* board, const bet_type_t& bet_type);

/*
Creates the g_all_prwin_vs_any_hand global.
This variable is used to calculate some probabilities fast.
It should not be used directly.

Parameters: none

Returns: std::vector<float> - g_all_prwin_vs_any_hand
*/
std::vector<float>
create_all_prwin_vs_any_hand_flop();
