#include "hero_preflop_range.h"
#include "poker_exception.h"
#include "position.h"
#include "util.h"
#include "hand.h"
#include <vector>
#include <cassert>
#ifdef _DEBUG
#include "clean_windows.h"
#endif

extern std::vector<const hero_preflop_range_t*> g_open_hero_preflop_ranges, g_facing_raise_hero_preflop_ranges,
									g_facing_3bet_hero_preflop_ranges, g_facing_4bet_hero_preflop_ranges;


static std::vector<std::string>
get_position_map();

static hero_preflop_range_t*
get_range_from_csv(const std::string& file_name);

static const hero_preflop_range_t* 
get_open_hero_preflop_range(position_t hero_position);

static const hero_preflop_range_t*
get_facing_raise_hero_preflop_range(position_t hero_position, position_t villain_position);

static const hero_preflop_range_t*
get_facing_3bet_hero_preflop_range(position_t hero_position, position_t villain_position);

static const hero_preflop_range_t* 
get_facing_4bet_hero_preflop_range(position_t hero_position, position_t villain_position);


void 
apply_raise_prob(hero_preflop_range_hand_t* range_hand)
{
	if (range_hand->raise_prob > generate_random())
	{
		range_hand->hand_action = RAISE;
		range_hand->raise_prob = 1;
	}
}


hero_preflop_range_t::~hero_preflop_range_t()
{
	for (auto* range_hand : hero_preflop_range)
	{
		delete range_hand;
	}
}


void 
hero_preflop_range_t::add(const hand_t* hand, hand_action_t hand_action, float raise_prob)
{
	hero_preflop_range_hand_t* range_hand = new hero_preflop_range_hand_t;
	range_hand->hand = hand;
	range_hand->hand_action = hand_action;
	range_hand->raise_prob = raise_prob;
	hero_preflop_range.emplace_back(range_hand);
}


void
hero_preflop_range_t::remove(const hand_t* hand)
{
	for(size_t i = 0; i < hero_preflop_range.size(); ++i)
	{
		if (are_similar_hands(hand, hero_preflop_range[i]->hand))
		{
			std::swap(hero_preflop_range[i], hero_preflop_range[hero_preflop_range.size() - 1]);
			delete hero_preflop_range.back();
			hero_preflop_range.pop_back();
			return;
		}
	}
}


bool
hero_preflop_range_t::contains(const hand_t* hand) const
{
	for (size_t i = 0; i < hero_preflop_range.size(); ++i)
	{
		if (are_similar_hands(hand, hero_preflop_range[i]->hand))
		{
			return true;
		}
	}
	return false;
}


hero_preflop_range_hand_t*
hero_preflop_range_t::fetch(const hand_t* hand) const
{
	for (size_t i = 0; i < hero_preflop_range.size(); ++i)
	{
		if (are_similar_hands(hand, hero_preflop_range[i]->hand))
		{
			return hero_preflop_range[i];
		}
	}
	throw poker_exception_t("hero_preflop_range_t::fetch: hand not in range");
}


size_t
hero_preflop_range_t::size() const
{
	return hero_preflop_range.size();
}


hero_preflop_range_t*
copy_hero_preflop_range(const hero_preflop_range_t* hero_preflop_range)
{
	hero_preflop_range_t* new_range = new hero_preflop_range_t;
	for (auto* range_hand : hero_preflop_range->hero_preflop_range)
	{
		new_range->add(range_hand->hand, range_hand->hand_action, range_hand->raise_prob);
	}
	return new_range;
}


std::vector<const hero_preflop_range_t*> 
create_open_hero_preflop_ranges()
{
	std::vector<const hero_preflop_range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t position = UTG; position < BB; ++position)
	{
		std::string file_name = "preflop_range\\" + position_map[position] + "_Open.csv";
		hero_preflop_range_t* hero_preflop_range = get_range_from_csv(file_name);
		ranges.emplace_back(hero_preflop_range);
	}
	return ranges;
}


std::vector<const hero_preflop_range_t*>
create_facing_raise_hero_preflop_ranges()
{
	std::vector<const hero_preflop_range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t hero_pos = UTG; hero_pos <= BB; ++hero_pos)
	{
		for (position_t villain_pos = UTG; villain_pos < hero_pos; ++villain_pos)
		{
			std::string file_name = "preflop_range\\" + position_map[hero_pos]+
				"_" + position_map[villain_pos] + "_Facing_Raise.csv";
			hero_preflop_range_t* hero_preflop_range = get_range_from_csv(file_name);
			ranges.emplace_back(hero_preflop_range);
		}
	}
	return ranges;
}


std::vector<const hero_preflop_range_t*>
create_facing_3bet_hero_preflop_ranges()
{
	std::vector<const hero_preflop_range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t hero_pos = UTG; hero_pos <= BB; ++hero_pos)
	{
		for (position_t villain_pos = hero_pos + 1; villain_pos <= BB; ++villain_pos)
		{
			std::string file_name = "preflop_range\\" + position_map[hero_pos]+
				"_" + position_map[villain_pos] + "_Facing_3bet.csv";
			hero_preflop_range_t* hero_preflop_range = get_range_from_csv(file_name);
			ranges.emplace_back(hero_preflop_range);
		}
	}
	return ranges;
}


std::vector<const hero_preflop_range_t*>
create_facing_4bet_hero_preflop_ranges()
{
	std::vector<const hero_preflop_range_t*> ranges;
	auto position_map = get_position_map();
	for (position_t hero_pos = UTG; hero_pos <= BB; ++hero_pos)
	{
		for (position_t villain_pos = UTG; villain_pos < hero_pos; ++villain_pos)
		{
			std::string file_name = "preflop_range\\" + position_map[hero_pos]+
				"_" + position_map[villain_pos] + "_Facing_4bet.csv";
			hero_preflop_range_t* hero_preflop_range = get_range_from_csv(file_name);
			ranges.emplace_back(hero_preflop_range);
		}
	}
	return ranges;
}


hero_preflop_range_t*
get_hero_preflop_range(position_t hero_position, position_t villain_position, bet_type_t bet_type)
{
	DLOG(INFO, ("getting range for: hero=" + std::to_string(hero_position) + 
		" villain=" + std::to_string(villain_position) +
		" bet_type=" + std::to_string(bet_type)).c_str());
	const hero_preflop_range_t* hero_preflop_range = nullptr;
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
			hero_preflop_range = get_open_hero_preflop_range(hero_position);
		}
		break;
	case FACING_RAISE:
		if ((int)hero_position < (int)villain_position)
		{
			bet_type = FACING_3BET;
		}
		else
		{
			hero_preflop_range = get_facing_raise_hero_preflop_range(hero_position, villain_position);
		}
		break;
	case FACING_3BET:
		hero_preflop_range =  get_facing_3bet_hero_preflop_range(hero_position, villain_position);
		break;
	case FACING_4BET:
		if ((int)hero_position < (int)villain_position)
		{
			hero_position = SB;
			villain_position = UTG;
		}
		hero_preflop_range = get_facing_4bet_hero_preflop_range(hero_position, villain_position);
		break;
	default:
		throw poker_exception_t("get_range: Invalid bet_type");
	}
	return copy_hero_preflop_range(hero_preflop_range);
}


const hero_preflop_range_t*
get_open_hero_preflop_range(position_t hero_position)
{
	return g_open_hero_preflop_ranges[hero_position];
}


const hero_preflop_range_t*
get_facing_raise_hero_preflop_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	assert(villain_position_int >= UTG && 
		villain_position_int < hero_position_int && hero_position_int <= BB);
	int ind = (hero_position_int - 1) * (hero_position_int) / 2 + villain_position_int;
	return g_facing_raise_hero_preflop_ranges[ind];
}


const hero_preflop_range_t*
get_facing_3bet_hero_preflop_range(position_t hero_position, position_t villain_position)
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
	return g_facing_3bet_hero_preflop_ranges[ind];
}


const hero_preflop_range_t*
get_facing_4bet_hero_preflop_range(position_t hero_position, position_t villain_position)
{
	int hero_position_int = (int)hero_position;
	int villain_position_int = (int)villain_position;
	assert(villain_position_int >= UTG &&
		villain_position_int < hero_position_int && hero_position_int <= BB);
	int ind = (hero_position_int - 1) * (hero_position_int) / 2 + villain_position_int;
	return g_facing_4bet_hero_preflop_ranges[ind];
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


static hero_preflop_range_t*
get_range_from_csv(const std::string& file_name)
{
	FILE* csv = fopen(file_name.c_str(), "r");
	if (csv == NULL)
	{
		throw poker_exception_t("get_range_from_csv: can't find file");
	}
	hero_preflop_range_t* hero_preflop_range = new hero_preflop_range_t;
	if (hero_preflop_range == NULL)
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
		hero_preflop_range->add(hand, hand_action_true, (float)raise_percent / 100);
	}
	fclose(csv);
	return hero_preflop_range;
}


