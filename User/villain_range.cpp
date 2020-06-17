#include "villain_range.h"
#include "util.h"
#include "poker_exception.h"
#include <algorithm>
#include <fstream>
#include <cassert>

extern std::vector<const villain_range_t*> g_open_villain_preflop_range, g_call_villain_preflop_range;
extern const villain_range_t* g_reraise_villain_preflop_range;


static villain_range_t*
get_range_from_csv(const std::string& file_name);


static std::vector<std::string>
get_position_map();


void
villain_range_t::add(const hand_t* hand)
{
	villain_range.insert(hand);
}


void
villain_range_t::remove(const std::vector<const hand_t*>& hands)
{
	std::set<const hand_t*> hands_set(all(hands));
	std::set<const hand_t*> diff;
	std::set_difference(all(villain_range), all(hands_set), std::inserter(diff, diff.begin()));
	villain_range = diff;
}


bool
villain_range_t::constains_at_least_one(const std::vector<const hand_t*>& hands)
{
	for (const auto* hand : hands)
	{
		if (villain_range.count(hand))
		{
			return true;
		}
	}
	return false;
}


villain_range_t*
copy_villain_range(const villain_range_t* villain_range)
{
	villain_range_t* villain_range_copy = new villain_range_t;
	villain_range_copy->villain_range = villain_range->villain_range;
	return villain_range_copy;
}


std::vector<const villain_range_t*>
create_open_villain_preflop_range()
{
	std::vector<const villain_range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t pos = UTG; pos < BB; ++pos)
	{
		std::string file_name = "villain_range\\" + position_map[pos] + "_Open.csv";
		const auto* range = get_range_from_csv(file_name);
		ranges.push_back(range);
	}
	return ranges;
}


std::vector<const villain_range_t*>
create_call_villain_preflop_range()
{
	std::vector<const villain_range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t pos = UTG; pos <= BB; ++pos)
	{
		std::string file_name = "villain_range\\" + position_map[pos] + "_Call.csv";
		const auto* range = get_range_from_csv(file_name);
		ranges.push_back(range);
	}
	return ranges;
}


const villain_range_t*
create_reraise_villain_preflop_range()
{
	return get_range_from_csv("villain_range\\All_Reraise.csv");
}


villain_range_t*
get_villain_prelop_range(position_t villain_position, hand_action_t last_villain_action)
{
	if (last_villain_action == RERAISE)
	{
		return copy_villain_range(g_reraise_villain_preflop_range);
	}
	else if (last_villain_action == RAISE)
	{
		assert(villain_position != BB);
		return copy_villain_range(g_open_villain_preflop_range[(int)villain_position]);
	}
	else if (last_villain_action == CALL)
	{
		return copy_villain_range(g_call_villain_preflop_range[(int)villain_position]);
	}
	throw poker_exception_t("get_villain_preflop_range: invalid last_villain_action");
}


static villain_range_t*
get_range_from_csv(const std::string& file_name)
{
	std::ifstream file(file_name);
	std::string hand_string;
	std::vector<char> color_chars = {'s', 'd', 'c', 'h'};
	villain_range_t* range = new villain_range_t;
	while (file >> hand_string)
	{
		std::string card_strs[2];
		std::vector<const hand_t*> hands;
		card_strs[0].push_back(hand_string[0]);
		card_strs[1].push_back(hand_string[1]);
		card_strs[0].push_back('x');
		card_strs[1].push_back('x');
		if (hand_string[2] == 'o')
		{
			for (const auto& color_char_0 : color_chars)
			{
				for (const auto& color_char_1 : color_chars)
				{
					if (color_char_0 != color_char_1)
					{
						card_strs[0][1] = color_char_0;
						card_strs[1][1] = color_char_1;
						hands.push_back(get_hand(get_card(card_strs[0]), get_card(card_strs[1])));
					}
				}
			}
		}
		else if (hand_string[2] == 's')
		{
			for (const auto& color_char : color_chars)
			{
				card_strs[0][1] = card_strs[1][1] = color_char;
				hands.push_back(get_hand(get_card(card_strs[0]), get_card(card_strs[1])));
			}

		}
		else
		{
			throw poker_exception_t("get_range_from_csv: invalid hand string");
		}
		for (auto* hand : hands)
		{
			range->add(hand);
		}
	}
	return range;
}


// TODO: this function is repeated on this source and hero_preflop_range.cpp.
//       Find a better place for it
static std::vector<std::string>
get_position_map()
{
	std::vector<std::string> position_map(POSITION_COUNT);
	position_map[BB] = "BB";
	position_map[SB] = "SB";
	position_map[BTN] = "BTN";
	position_map[CO] = "CO";
	position_map[HJ] = "HJ";
	position_map[UTG] = "UTG";
	return position_map;
}
