#pragma once
#include "hand.h"
#include "position.h"
#include "board_derived_info.h"
#include <vector>


struct hero_preflop_range_hand_t
{
	const hand_t* hand = nullptr;
	hand_action_t hand_action = INVALID_HAND_ACTION;
	/*
	This is the probability that a raise will happen instead of hand_action.
	Has a value between [0, 1].
	*/ 
	float raise_prob = 0;

	bool
	operator<(const hero_preflop_range_hand_t& other) const
	{
		return *hand < *other.hand;
	}
};

/*
Applies the raise probability found in a hero_preflop_range_hand_t.
The hand action will change to RAISE, based on raise_prob.

Parameters: OUT hero_preflop_range_hand_t*
*/
void 
apply_raise_prob(hero_preflop_range_hand_t* range_hand);


struct hero_preflop_range_t
{
	std::vector<hero_preflop_range_hand_t*> hero_preflop_range;

	// The destructor frees the elements accordingly.
	~hero_preflop_range_t();

	/*
	Adds a range_hand_t* to the range, constructed from the parameters.

	Parameters: hand_t* hand -- a pointer to the hand
				hand_action_t hand_action -- the action that should be made
				float raise_prob -- the probability that a raise will happen
									instead of hand_action

	Returns: none
	*/
	void
	add(const hand_t* hand, hand_action_t hand_action = FOLD, float raise_prob = 0);


	/*
	Removes and frees a range_hand_t* from range, according to the values from hand.

	Parameters: hand_t* hand -- the range_hand_t removed has a similar value with this.
	
	Returns: none
	*/
	void
	remove(const hand_t* hand);


	/*
	Checks if the hand is in range.

	Parameters: hand_t* hand -- the range_hand_t to check has a similar value with this.

	Retuns: bool -- true if the hand is in range and false otherwise
	*/
	bool
	contains(const hand_t* hand) const;


	/*
	Gets the range_hand_t based on given hand.

	Parameters: hand_t* hand -- the range_hand_t to fetch has a similar value with this

	Returns: range_hand_t* -- the appropriate range. Should not be freed. 
	*/
	hero_preflop_range_hand_t* 
	fetch(const hand_t* hand) const;


	/*
	The size of the range.

	Parameters: none

	Returns: size_t -- the size of the range
	*/
	size_t
	size() const;
};


/*
Copies the given range, such that it can be modified. Should be called everytime
a get_*_range function is called  with the intention to modify the range.

Parameters: IN range_t* range -- the range to copy

Returns: range_t* -- a pointer to the new range. 
					Should be deleted when it is no longer needed.
*/
hero_preflop_range_t*
copy_hero_preflop_range(const hero_preflop_range_t* hero_preflop_range);


/*
Creates the ranges that correspond with the 
open/facing_raise/facing_3bet/facing_4bet positions.
These are fetched from "prefop_range" directory.

These functions should only be called once, when the process is attaching.
*/
std::vector<const hero_preflop_range_t*>
create_open_hero_preflop_ranges();

std::vector<const hero_preflop_range_t*>
create_facing_raise_hero_preflop_ranges();

std::vector<const hero_preflop_range_t*>
create_facing_3bet_hero_preflop_ranges();

std::vector<const hero_preflop_range_t*>
create_facing_4bet_hero_preflop_ranges();

// TODO: add destroy functions for ranges 


/*
Gets the range of hero according to its position, its attacker and 
the bet_type. This is a copy of the gloval variable.

For
	bet_type == OPEN: villain_position can be anything. There is not open range for BB.
	bet_type == FACING_RAISE: hero_position < villain_position
	bet_type == FACING_3BET: hero_position > villain_position
	bet_Type == FACING_4BET: hero_position < villain_position
*/
hero_preflop_range_t*
get_hero_preflop_range(position_t hero_position, position_t villain_position, bet_type_t bet_type);
