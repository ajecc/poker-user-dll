#pragma once
#include "hand.h"
#include <set>
#include <vector>


struct range_t
{
	struct range_comp_t
	{
		bool operator()(const hand_t* lhs, const hand_t* rhs)
		{
			return *lhs < *rhs;
		}
	};

	std::set<hand_t*, range_comp_t> range;

	void add(hand_t* hand);

	void remove(hand_t* hand);

	bool contains(hand_t* hand);
};




std::vector<range_t*> create_hero_open_range();
