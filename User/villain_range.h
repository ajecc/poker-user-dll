#pragma once
#include <set>
#include "hand.h"
#include "position.h"

enum villain_action_t
{
	INVALID_VILLAIN_ACTION = -1,
	VILLAIN_ACTION_LIMP = 0,
	VILLAIN_ACTION_CHECK = 1,
	VILLAIN_ACTION_OPEN = 2,
	VILLAIN_ACTION_CALL = 3,
	VILLAIN_ACTION_RAISE = 4,
	VILLAIN_ACTION_COUNT = 5
};


struct villain_range_t
{
	std::set<const hand_t*> villain_range;

	/*
	Adds a hand to the villain's range

	Paramenters: IN hand_t* - the hand to add to the range
	*/
	void
	add(const hand_t* hand);


	/*
	Removes some hands from the villain's range.
	Not all hands need to exist in the range

	Parameters: std::vector<const hand_t*> - the hands to remove
	*/
	void
	remove(const std::vector<const hand_t*>& hands);

	
	/*
	Removes all the hands from the villain's range, containing a certain card

	Parameters: card_t* - the card to be removed from the range
	*/
	void
	remove_containg_card(const card_t* card);


	/*
	Checks if the range contains at least one of the hands
	passed to this function

	Parameters: std::vector<const hand_t*> - the hands to be checked

	Returns: bool - true if at least one hand is in the villain's range
					and false otherwise
	*/
	bool
	contains_at_least_one(const std::vector<const hand_t*>& hands) const;


	std::string
	to_string() const;
};


/*
Updates the range of the villain, based on the board
and certain criterias found in player_t structure

Parameters: OUT player_t* - the player whose range should be updated
			IN board_t* - the board
*/
void
update_player_villain_range(player_t* player, const board_t* board);


/*
Copies the villain range. The copied range is dynamically allocated
and should be freed when no longer used

Paramters: IN villain_range_t* - the range to be copied

Returns: villain_range_t* - a pointer to the copy
*/
villain_range_t*
copy_villain_range(const villain_range_t* villain_range);


/*
The functions below load the ranges corresponding 
to certain stages of the game, from some CSVs.
These will be stored on some global variables,
to be easily accessed.
*/
std::vector<const villain_range_t*>
create_open_villain_preflop_range();

std::vector<const villain_range_t*>
create_call_villain_preflop_range();

const villain_range_t*
create_raise_villain_preflop_range();

const villain_range_t*
create_limp_villain_preflop_range();

const villain_range_t*
create_check_villain_preflop_range();


/*
Gets the villain's preflop range, using all the global variables
specific to the preflop ranges. The range can be modified, but
should be freed when no longer used.

Parameters: IN position_t - the villain's position
			IN villain_action_t - the villain's action

Returns: villain_range_t* - a copy of the villain's range.
*/
villain_range_t*
get_villain_preflop_range(const position_t& villain_position, const villain_action_t& last_villain_action);


/*
Checks if a hand has gutshot

Parameters: IN hand_t* - the hand to be checked for a gutshot
			IN board_t* - the board 

Returns: bool - true if the hand has a gutshot and false otherwise
*/
bool
has_gutshot(const hand_t* hand, const board_t* board);


/*
Checks if a hand has open ender

Parameters: IN hand_t* - the hand to be checked for a open ender
			IN board_t* - the board 

Returns: bool - true if the hand has a open ender and false otherwise
*/
bool
has_open_ender(const hand_t* hand, const board_t* board);


/*
Checks if a hand has flush draw

Parameters: IN hand_t* - the hand to be checked for a flush draw
			IN board_t* - the board 

Returns: bool - true if the hand has a flush draw and false otherwise
*/
bool
has_flush_draw(const hand_t* hand, const board_t* board);
