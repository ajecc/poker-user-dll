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
	RERAISE = 4,
	HAND_ACTION_COUNT = 5
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
Gets the index of the hand, as it is found in g_all_hands.
The hand is defined by two cards

Parameters: IN card_t* - the first card in the hand
			IN card_t* - the second card in the hand

Returns: size_t - the index of hand, as found in g_all_hands
*/
size_t
get_hand_index(const card_t* card_0, const card_t* card_1);


/*
Gets the index of the hand, as it is found in g_all_hands

Parameters: IN hand_t* - the hand

Returns: size_t - the index of hand, as found in g_all_hands
*/
size_t
get_hand_index(const hand_t* hand);


/*
Gets a pointer to the hand that contains the 2 cards given.

Parameters: IN card_t* card_0
			IN card_t* card_1

Returns: hand_t* -- pointer to the hand that contains the cards, fetched from 
					g_all_hands. Should not be freed.
*/
const hand_t*
get_hand(const card_t* card_0, const card_t* card_1);


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
