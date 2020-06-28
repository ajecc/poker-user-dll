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

	
	void
	add(const hand_t* hand);


	void
	remove(const std::vector<const hand_t*>& hands);

	
	void
	remove_containg_card(const card_t* card);


	bool
	contains_at_least_one(const std::vector<const hand_t*>& hands) const;


	std::string
	to_string() const;
};


void
update_player_villain_range(player_t* player, const board_t* board);


villain_range_t*
copy_villain_range(const villain_range_t* villain_range);


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


villain_range_t*
get_villain_preflop_range(const position_t& villain_position, const villain_action_t& last_villain_action);
