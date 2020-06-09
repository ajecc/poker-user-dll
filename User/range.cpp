#include "range.h"
#include "poker_exception.h"
#include "position.h"
#include "util.h"
#include "hand.h"
#include <vector>
#include <cassert>
#ifdef _DEBUG
#include "clean_windows.h"
#endif

extern std::vector<const range_t*> g_open_ranges, g_facing_raise_ranges,
									g_facing_3bet_ranges, g_facing_4bet_ranges;

static std::vector<std::string>
get_position_map();

static range_t*
get_range_from_csv(const std::string& file_name);


void 
apply_raise_prob(range_hand_t* range_hand)
{
	if (range_hand->raise_prob > generate_random())
	{
		range_hand->hand_action = RAISE;
		range_hand->raise_prob = 1;
	}
}


range_t::~range_t()
{
	for (auto* range_hand : range)
	{
		delete range_hand;
	}
}


void 
range_t::add(const hand_t* hand, hand_action_t hand_action, float raise_prob)
{
	range_hand_t* range_hand = new range_hand_t;
	range_hand->hand = hand;
	range_hand->hand_action = hand_action;
	range_hand->raise_prob = raise_prob;
	range.emplace_back(range_hand);
}


void
range_t::remove(const hand_t* hand)
{
	for(size_t i = 0; i < range.size(); ++i)
	{
		if (are_similar_hands(hand, range[i]->hand))
		{
			std::swap(range[i], range[range.size() - 1]);
			delete range.back();
			range.pop_back();
			return;
		}
	}
}


bool
range_t::contains(const hand_t* hand) const
{
	for (size_t i = 0; i < range.size(); ++i)
	{
		if (are_similar_hands(hand, range[i]->hand))
		{
			return true;
		}
	}
	return false;
}


range_hand_t*
range_t::fetch(const hand_t* hand) const
{
	for (size_t i = 0; i < range.size(); ++i)
	{
		if (are_similar_hands(hand, range[i]->hand))
		{
			return range[i];
		}
	}
	throw poker_exception_t("range_t::fetch: hand not in range");
}


size_t
range_t::size() const
{
	return range.size();
}


range_t*
copy_range(const range_t* range)
{
	range_t* new_range = new range_t;
	for (auto* range_hand : range->range)
	{
		new_range->add(range_hand->hand, range_hand->hand_action, range_hand->raise_prob);
	}
	return new_range;
}


std::vector<const range_t*> 
create_open_ranges()
{
	std::vector<const range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t position = UTG; position < BB; ++position)
	{
		std::string file_name = "preflop_range\\" + position_map[position] + "_Open.csv";
		range_t* range = get_range_from_csv(file_name);
		ranges.emplace_back(range);
	}
	return ranges;
}


std::vector<const range_t*>
create_facing_raise_ranges()
{
	std::vector<const range_t*> ranges;
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


std::vector<const range_t*>
create_facing_3bet_ranges()
{
	std::vector<const range_t*> ranges;
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


std::vector<const range_t*>
create_facing_4bet_ranges()
{
	std::vector<const range_t*> ranges;
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


const range_t*
get_range(position_t hero_position, position_t villain_position, bet_type_t bet_type)
{
	DLOG(INFO, ("getting range for: hero=" + std::to_string(hero_position) + 
		" villain=" + std::to_string(villain_position) +
		" bet_type=" + std::to_string(bet_type)).c_str());
	// TODO: add better handling
	switch (bet_type)
	{
	case OPEN:
		if (hero_position == BB)
		{
			villain_position = SB;
			bet_type = FACING_RAISE;
		}
		else
		{
			return get_open_range(hero_position);
		}
	case FACING_RAISE:
		if ((int)hero_position < (int)villain_position)
		{
			bet_type = FACING_3BET;
		}
		else
		{
			return get_facing_raise_range(hero_position, villain_position);
		}
	case FACING_3BET:
		return get_facing_3bet_range(hero_position, villain_position);
	case FACING_4BET:
		if ((int)hero_position < (int)villain_position)
		{
			hero_position = SB;
			villain_position = UTG;
		}
		return get_facing_4bet_range(hero_position, villain_position);
	default:
		throw poker_exception_t("get_range: Invalid bet_type");
	}
}


const range_t*
get_open_range(position_t hero_position)
{
	return g_open_ranges[hero_position];
}


const range_t*
get_facing_raise_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	assert(villain_position_int >= UTG && 
		villain_position_int < hero_position_int && hero_position_int <= BB);
	int ind = (hero_position_int - 1) * (hero_position_int) / 2 + villain_position_int;
	return g_facing_raise_ranges[ind];
}


const range_t*
get_facing_3bet_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	int position_count_int = (int)POSITION_COUNT;
	assert(hero_position_int >= UTG &&
		hero_position_int < villain_position_int && villain_position_int <= BB);
	int ind = position_count_int * (position_count_int - 1) / 2;
	ind += villain_position_int - hero_position_int - 1;
	hero_position_int = position_count_int - hero_position_int - 1;
	ind -= (hero_position_int + 1) * hero_position_int / 2;
	return g_facing_3bet_ranges[ind];
}


const range_t*
get_facing_4bet_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	assert(villain_position_int >= UTG &&
		villain_position_int < hero_position_int && hero_position_int <= BB);
	int ind = (hero_position_int - 1) * (hero_position_int) / 2 + villain_position_int;
	return g_facing_4bet_ranges[ind];
}

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


static range_t*
get_range_from_csv(const std::string& file_name)
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
	while (fscanf(csv, "%3[^,],%c,%d\n",
		(char*)cards.c_str(), &hand_action, &raise_percent) != EOF)
	{
		std::string card_str;
		card_str.push_back(cards[0]);
		card_str.push_back('h');
		const card_t* card_lhs = get_card(card_str);
		card_str = "";
		card_str.push_back(cards[1]);
		if (cards[2] == 's')
		{
			card_str.push_back('h');
		}
		else
		{
			card_str.push_back('c');
		}
		const card_t* card_rhs = get_card(card_str);
		const hand_t* hand = get_hand(card_lhs, card_rhs);
		hand_action_t hand_action_true = get_hand_action_from_char(hand_action);
		range->add(hand, hand_action_true, (float)raise_percent / 100);
	}
	fclose(csv);
	return range;
}


