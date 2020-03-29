#include "range.h"
#include "poker_exception.h"
#include "position.h"
#include <vector>
#include <cassert>

extern std::vector<range_t*> g_open_ranges, g_facing_raise_ranges, g_facing_3bet_ranges, g_facing_4bet_ranges;

static std::vector<std::string> get_position_map();

static range_t* get_range_from_csv(const std::string& file_name);


void range_t::add(hand_t* hand, hand_action_t hand_action, double raise_prob)
{
	range_hand_t* range_hand = new range_hand_t;
	range_hand->hand = hand;
	range_hand->hand_action = hand_action;
	range_hand->raise_prob = raise_prob;
	range.insert(range_hand);
}


void range_t::remove(hand_t* hand)
{
	range_hand_t range_hand;
	range_hand.hand = hand;
	auto range_hand_to_erase = range.find(&range_hand);
	if (range_hand_to_erase == range.end())
	{
		return;
	}
	auto* range_hand_to_erase_val = *range_hand_to_erase;	
	range.erase(range_hand_to_erase);
	delete range_hand_to_erase_val;
}


bool range_t::contains(hand_t* hand)
{
	range_hand_t range_hand; 
	range_hand.hand = hand;
	return (range.find(&range_hand) != range.end());
}


std::vector<range_t*> create_open_ranges()
{
	std::vector<range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t position = UTG; position < BB; ++position)
	{
		std::string file_name = "preflop_range\\" + position_map[position] + "_Open.csv";
		range_t* range = get_range_from_csv(file_name);
		ranges.emplace_back(range);
	}
	return ranges;
}


std::vector<range_t*> create_facing_raise_ranges()
{
	std::vector<range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t hero_pos = UTG; hero_pos <= BB; ++hero_pos)
	{
		for (position_t villain_pos = UTG; villain_pos < hero_pos; ++villain_pos)
		{
			std::string file_name = "preflop_range\\" + position_map[hero_pos]+
				"_" + position_map[villain_pos] + "_Facing_Raise.csv";
			range_t* range = get_range_from_csv(file_name);
			ranges.emplace_back(range);
		}
	}
	return ranges;
}


std::vector<range_t*> create_facing_3bet_ranges()
{
	std::vector<range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t hero_pos = UTG; hero_pos <= BB; ++hero_pos)
	{
		for (position_t villain_pos = hero_pos + 1; villain_pos <= BB; ++villain_pos)
		{
			std::string file_name = "preflop_range\\" + position_map[hero_pos]+
				"_" + position_map[villain_pos] + "_Facing_3bet.csv";
			range_t* range = get_range_from_csv(file_name);
			ranges.emplace_back(range);
		}
	}
	return ranges;
}


std::vector<range_t*> create_facing_4bet_ranges()
{
	std::vector<range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t hero_pos = UTG; hero_pos <= BB; ++hero_pos)
	{
		for (position_t villain_pos = UTG; villain_pos < hero_pos; ++villain_pos)
		{
			std::string file_name = "preflop_range\\" + position_map[hero_pos]+
				"_" + position_map[villain_pos] + "_Facing_4bet.csv";
			range_t* range = get_range_from_csv(file_name);
			ranges.emplace_back(range);
		}
	}
	return ranges;
}


range_t* get_open_range(position_t hero_position)
{
	return g_open_ranges[hero_position];
}


range_t* get_facing_raise_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	assert(villain_position_int >= UTG && villain_position_int < hero_position_int && hero_position_int <= BB);
	int ind = (hero_position_int - 1) * (hero_position_int - 2) / 2 + villain_position_int;
	return g_facing_raise_ranges[ind];
}


range_t* get_facing_3bet_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	int position_count_int = (int)POSITION_COUNT;
	assert(hero_position_int >= UTG && hero_position_int < villain_position_int && villain_position_int <= BB);
	int ind = position_count_int * (position_count_int - 1) / 2;
	ind += villain_position_int - hero_position_int - 1;
	hero_position_int = position_count_int - hero_position_int - 1;
	ind -= (hero_position_int + 1) * hero_position_int / 2;
	return g_facing_3bet_ranges[ind];
}


range_t* get_facing_4bet_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	assert(villain_position_int >= UTG && villain_position_int < hero_position_int && hero_position_int <= BB);
	int ind = (hero_position_int - 1) * (hero_position_int - 2) / 2 + villain_position_int;
	return g_facing_4bet_ranges[ind];
}


static std::vector<std::string> get_position_map()
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


static range_t* get_range_from_csv(const std::string& file_name)
{
	FILE* csv = fopen(file_name.c_str(), "r");
	if (csv == NULL)
	{
		throw poker_exception_t("get_range_from_csv: can't find file");
	}
	range_t* range = new range_t;
	if (range == NULL)
	{
		fclose(csv);
		throw poker_exception_t("get_range_from_csv: new range_t failed");
	}
	std::string cards = "XXX";
	char hand_action;
	int raise_percent;
	while (fscanf(csv, "%3[^,],%c,%d", (char*)cards.c_str(), &hand_action, &raise_percent) != EOF)
	{
		std::string card_str;
		card_str.push_back(cards[0]);
		card_str.push_back('h');
		card_t* card_lhs = get_card_from_string(card_str);
		card_str = "";
		card_str.push_back(cards[1]);
		if (card_str[2] == 's')
		{
			card_str.push_back('h');
		}
		else
		{
			card_str.push_back('c');
		}
		card_t* card_rhs = get_card_from_string(card_str);
		hand_t* hand = get_hand(card_lhs, card_rhs);
		hand_action_t hand_action_true = get_hand_action_from_char(hand_action);
		range->add(hand, hand_action_true, (double)raise_percent / 100);
	}
	fclose(csv);
	return range;
}
