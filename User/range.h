#pragma once
#include "hand.h"
#include "position.h"
#include <set>
#include <vector>

struct range_hand_t
{
	hand_t* hand;
	hand_action_t hand_action;
	double raise_prob;

	bool operator<(const range_hand_t& other) const
	{
		return *hand < *other.hand;
	}
};

struct range_t
{
	struct range_comp_t
	{
		bool operator()(const range_hand_t* lhs, const range_hand_t* rhs) const
		{
			return *lhs < *rhs;
		}
	};


	std::set<range_hand_t*, range_comp_t> range;

	void add(hand_t* hand, hand_action_t hand_action = FOLD, double raise_prob = 0);

	void remove(hand_t* hand);

	bool contains(hand_t* hand);
};



std::vector<range_t*> create_open_ranges();

std::vector<range_t*> create_facing_raise_ranges();

std::vector<range_t*> create_facing_3bet_ranges();

std::vector<range_t*> create_facing_4bet_ranges();

// TODO: add destroy functions for ranges 


range_t* get_open_range(position_t hero_position);

range_t* get_facing_raise_range(position_t hero_position, position_t villain_position);

range_t* get_facing_3bet_range(position_t hero_position, position_t villain_position);

range_t* get_facing_4bet_range(position_t hero_position, position_t villain_position);
