#pragma once
#include <set>
#include "hand.h"
#include "position.h"


struct villain_range_t
{
	std::set<const hand_t*> villain_range;

	
	void
	add(const hand_t* hand);


	void
	remove(const std::vector<const hand_t*>& hands);


	bool
	constains_at_least_one(const std::vector<const hand_t*>& hands);
};


villain_range_t*
copy_villain_range(const villain_range_t* villain_range);


std::vector<const villain_range_t*>
create_open_villain_preflop_range();

std::vector<const villain_range_t*>
create_call_villain_preflop_range();

const villain_range_t*
create_reraise_villain_preflop_range();


villain_range_t*
get_villain_preflop_range(position_t villain_position, hand_action_t last_villain_action);
