#pragma once
#include "hand.h"
#include "position.h"
#include "board_derived_info.h"
#include <vector>


struct range_hand_t
{
	hand_t* hand;
	hand_action_t hand_action;
	/*
	This is the probability that a raise will happen instead of hand_action.
	Has a value between [0, 1].
	*/ 
	float raise_prob;

	bool operator<(const range_hand_t& other) const
	{
		return *hand < *other.hand;
	}
};


void apply_raise_prob(range_hand_t* range_hand);


struct range_t
{
	std::vector<range_hand_t*> range;

	// The destructor frees the elements accordingly.
	~range_t();

	/*
	Adds a range_hand_t* to the range, constructed from the parameters.

	Parameters: hand_t* hand -- a pointer to the hand
				hand_action_t hand_action -- the action that should be made
				float raise_prob -- the probability that a raise will happen instead of hand_action

	Returns: none
	*/
	void add(hand_t* hand, hand_action_t hand_action = FOLD, float raise_prob = 0);


	/*
	Removes and frees a range_hand_t* from range, according to the values from hand.

	Parameters: hand_t* hand -- the range_hand_t removed has a similar value with this.
	
	Returns: none
	*/
	void remove(hand_t* hand);


	/*
	Checks if the hand is in range.

	Parameters: hand_t* hand -- the range_hand_t to check has a similar value with this.

	Retuns: bool -- true if the hand is in range and false otherwise
	*/
	bool contains(hand_t* hand);


	/*
	Gets the range_hand_t based on given hand.

	Parameters: hand_t* hand -- the range_hand_t to fetch has a similar value with this

	Returns: range_hand_t* -- the appropriate range. Should not be freed. 
	*/
	range_hand_t* fetch(hand_t* hand);


	/*
	The size of the range.

	Parameters: none

	Returns: size_t -- the size of the range
	*/
	size_t size();
};


/*
Copies the given range, such that it can be modified. Should be called everytime
a get_*_range function is called  with the intention to modify the range.

Parameters: IN range_t* range -- the range to copy

Returns: range_t* -- a pointer to the new range. Should be deleted when it is no longer needed.
*/
range_t* copy_range(range_t* range);


/*
Creates the ranges that correspond with the 
open/facing_raise/facing_3bet/facing_4bet positions.
These are fetched from "prefop_range" directory.

These functions should only be called once, when the process is attaching.
*/
std::vector<range_t*> create_open_ranges();
std::vector<range_t*> create_facing_raise_ranges();
std::vector<range_t*> create_facing_3bet_ranges();
std::vector<range_t*> create_facing_4bet_ranges();

// TODO: add destroy functions for ranges 


/*
Gets the range of hero according to its position, its attacker and 
the bet_type.

For
	bet_type == OPEN: villain_position can be anything. There is not open range for BB.
	bet_type == FACING_RAISE: hero_position < villain_position
	bet_type == FACING_3BET: hero_position > villain_position
	bet_Type == FACING_4BET: hero_position < villain_position
*/
range_t* get_range(position_t hero_position, position_t villain_position, bet_type_t bet_type);


/*
Superseted by get_range.
*/
range_t* get_open_range(position_t hero_position);
range_t* get_facing_raise_range(position_t hero_position, position_t villain_position);
range_t* get_facing_3bet_range(position_t hero_position, position_t villain_position);
range_t* get_facing_4bet_range(position_t hero_position, position_t villain_position);
