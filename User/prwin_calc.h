#pragma once
#include <vector>
#include "hand.h"

#define ALL_PRWIN_VS_ANY_HAND_FLOP_SHARED_NAME "Global\\ALL_PRWIN_VS_ANY_HAND_FLOP_SHARED"
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


std::vector<float>
create_all_prwin_vs_any_hand_flop();
