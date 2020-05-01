#pragma once

#include "card.h"
#include "board.h"
#include <vector>


const int HAND_CARD_COUNT = 2;

enum hand_action_t
{
	INVALID_HAND_ACTION = -1,
	FOLD = 0,
	CHECK = 1,
	CALL = 2,
	RAISE = 3,
	HAND_ACTION_COUNT = 4
};

/*
Transalates a char to hand_action_t. 
f = FOLD, c = CALL, r = RAISE. CHECK is not supported.

Parameters: char hand_action_char -- f,c or r

Returns: hand_action_t
*/
hand_action_t
get_hand_action_from_char(char hand_action_char);


struct hand_t
{
	const card_t* cards[HAND_CARD_COUNT] = { nullptr };
	bool suited = false;

	bool 
	operator<(const hand_t& other) const;

	std::string
	to_string() const;
};


/*
Gets a pointer to the hand that contains the 2 cards given.

Parameters: IN card_t* card_1
			IN card_t* card_2

Returns: hand_t* -- pointer to the hand that contains the cards, fetched from 
					g_all_hands. Should not be freed.
*/
const hand_t*
get_hand(const card_t* card_1, const card_t* card_2);


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
Checks if hand are simillar, i.e. they have the same rank and same suit.
At the moments, supports only hands fetched from g_all_hands.

Parameters: IN hand_t* lhs -- the first hand
			IN hand_t* rhs -- the other hand

Returns: bool - true if similar, false otherwise
*/
bool 
are_similar_hands(const hand_t* lhs, const hand_t* rhs);


/*
Creates all possible combinations of hands. The vector returned by this is a global one.
Hands can be fetched using get_hand.
This functions should only be called at attachment.

Parameters: none

Return: std::vector<hand_t*> -- the vector that contains all the hands.
								Should not be used explicitly (use get_hand instead)
*/
std::vector<const hand_t*>
create_all_hands();
