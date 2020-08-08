#include "villain_range.h"
#include "util.h"
#include "poker_exception.h"
#include "prwin_calc.h"
#include <algorithm>
#include <fstream>
#include <cassert>

extern std::vector<const villain_range_t*> g_open_villain_preflop_range, g_call_villain_preflop_range;
extern const villain_range_t* g_limp_villain_preflop_range;
extern const villain_range_t* g_check_villain_preflop_range;
extern const villain_range_t* g_raise_villain_preflop_range;


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
	if (villain_range.empty() || hands.empty())
	{
		return;
	}
	std::set<const hand_t*> hands_set(all(hands));
	std::set<const hand_t*> diff;
	std::set_difference(all(villain_range), all(hands_set), std::inserter(diff, diff.begin()));
	villain_range = diff;
}


void
villain_range_t::remove_containg_card(const card_t* card)
{
	std::vector<const hand_t*> hands_to_remove;
	for (const auto* villain_hand : villain_range)
	{
		if (*villain_hand->cards[0] == *card || *villain_hand->cards[1] == *card)
		{
			hands_to_remove.push_back(villain_hand);
		}
	}
	remove(hands_to_remove);
}


bool
villain_range_t::contains_at_least_one(const std::vector<const hand_t*>& hands) const
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


std::string
villain_range_t::to_string() const
{
	if (villain_range.empty())
	{
		return "[]";
	}
	std::string ret = "[";
	int cnt = 0;
	auto it = villain_range.end();
	it--;
	const auto* last_hand = *it;
	for (const auto* hand : villain_range)
	{
		ret += hand->to_string();
		if (hand != last_hand)
		{
			ret += ", ";
			cnt++;
			if (cnt == 6)
			{
				cnt = 0;
				ret += "\n";
			}
		}
	}
	return ret + "]";
}


void
update_player_villain_range(player_t* player, const board_t* board)
{
	// TODO: take bluffs in consideration
	if (board->stage == PREFLOP)
	{
		player->villain_range = get_villain_preflop_range(player->position, player->villain_action);
		player->villain_range->remove_containg_card(board->hero->hand->cards[0]);
		player->villain_range->remove_containg_card(board->hero->hand->cards[1]);
		return;
	}
	for (const auto* card : board->cards)
	{
		player->villain_range->remove_containg_card(card);
	}
	std::vector<const hand_t*> hands_to_remove;
	float players_cnt = (float)board->current_hand_players.size() - 1;
	auto normalize_prwin = [&players_cnt](const float& prwin)
		{
			return pow(prwin, players_cnt);
		};
	// at the moment we treat turn and flop the same
	if (board->stage == FLOP || board->stage == TURN)
	{
		if (board->current_hand_players.size() >= 3)
		{
			if (player->villain_action == VILLAIN_ACTION_CHECK)
			{
				for (const auto* hand : player->villain_range->villain_range)
				{
					if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) > 0.83f)
					{
						hands_to_remove.push_back(hand);
					}
				}
			}
			else if (player->villain_action == VILLAIN_ACTION_CALL)
			{
				for (const auto* hand : player->villain_range->villain_range)
				{
					if (!(has_flush_draw(hand, board) || has_open_ender(hand, board) ||
						pow(calc_prwin_vs_any_hand(hand, board), 2) > 0.5f))
					{
						hands_to_remove.push_back(hand);
					}
				}
			}
			else if (player->villain_action == VILLAIN_ACTION_OPEN || 
				player->villain_action == VILLAIN_ACTION_LIMP)
			{
				// TODO: this doesn't seem accurate. Correct it.
				if (!player->is_in_position || board->current_hand_players.size() >= 4)
				{
					for (const auto* hand : player->villain_range->villain_range)
					{
						if (!(has_flush_draw(hand, board) || has_open_ender(hand, board) ||
							pow(calc_prwin_vs_any_hand(hand, board), 2) > 0.6f))
						{
							hands_to_remove.push_back(hand);
						}
					}
				}
			}
			else if (player->villain_action == VILLAIN_ACTION_RAISE)
			{
				for (const auto* hand : player->villain_range->villain_range)
				{
					if (pow(calc_prwin_vs_any_hand(hand, board), 2) < 0.85f)
					{
						hands_to_remove.push_back(hand);
					}
				}
			}
		}
		else
		{
			if (player->villain_action == VILLAIN_ACTION_CHECK)
			{
				if (!player->is_in_position)
				{
					for (const auto* hand : player->villain_range->villain_range)
					{
						if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) > 0.85f &&
							normalize_prwin(calc_prwin_vs_any_hand(hand, board)) < 0.925f)
						{
							hands_to_remove.push_back(hand);
						}
					}
				}
			}
			else if (player->villain_action == VILLAIN_ACTION_CALL)
			{
				for (const auto* hand : player->villain_range->villain_range)
				{
					if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) < 0.44f &&
						!(has_flush_draw(hand, board) || has_gutshot(hand, board) || 
							has_open_ender(hand, board)))
					{
						hands_to_remove.push_back(hand);
					}
				}
			}
			else if (player->villain_action == VILLAIN_ACTION_OPEN
				|| player->villain_action == VILLAIN_ACTION_LIMP)
			{
				// TODO: this might not be accurate
				for (const auto* hand : player->villain_range->villain_range)
				{
					if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) < 0.6f &&
						!(has_flush_draw(hand, board) || has_gutshot(hand, board) || 
							has_open_ender(hand, board)))
					{
						hands_to_remove.push_back(hand);
					}
				}
			}
			else if (player->villain_action == VILLAIN_ACTION_RAISE)
			{
				for (const auto* hand : player->villain_range->villain_range)
				{
					if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) < 0.82f)
					{
						hands_to_remove.push_back(hand);
					}
				}
			}
		}
	}
	else if (board->stage == RIVER)
	{
		if (player->villain_action == VILLAIN_ACTION_CHECK)
		{
			for (const auto* hand : player->villain_range->villain_range)
			{
				if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) > 0.852f)
				{
					hands_to_remove.push_back(hand);
				}
			}
		}
		else if (player->villain_action == VILLAIN_ACTION_OPEN
			|| player->villain_action == VILLAIN_ACTION_LIMP)
		{
			for (const auto* hand : player->villain_range->villain_range)
			{
				if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) < 0.78f)
				{
					hands_to_remove.push_back(hand);
				}
			}
		}
		else if(player->villain_action == VILLAIN_ACTION_RAISE)
		{
			for (const auto* hand : player->villain_range->villain_range)
			{
				if (normalize_prwin(calc_prwin_vs_any_hand(hand, board)) < 0.92f)
				{
					hands_to_remove.push_back(hand);
				}
			}
		}
	}
	else
	{
		throw poker_exception_t("update_player_villain_range: invalid board stage");
	}
	player->villain_range->remove(hands_to_remove);
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
create_raise_villain_preflop_range()
{
	return get_range_from_csv("villain_range\\All_Raise.csv");
}


const villain_range_t*
create_limp_villain_preflop_range()
{
	return get_range_from_csv("villain_range\\All_Limp.csv");
}


const villain_range_t*
create_check_villain_preflop_range()
{
	return get_range_from_csv("villain_range\\All_Check.csv");
}


villain_range_t*
get_villain_preflop_range(const position_t& villain_position, const villain_action_t& last_villain_action)
{
	const villain_range_t* villain_range = nullptr;
	switch (last_villain_action)
	{
	case VILLAIN_ACTION_LIMP:
		assert(villain_position != BB);
		villain_range = g_limp_villain_preflop_range;
		break;
	case VILLAIN_ACTION_CHECK:
		assert(villain_position == BB);
		villain_range = g_check_villain_preflop_range;
		break;
	case VILLAIN_ACTION_OPEN:
		assert(villain_position != BB);
		villain_range = g_open_villain_preflop_range[(int)villain_position];
		break;
	case VILLAIN_ACTION_CALL:
		villain_range = g_call_villain_preflop_range[(int)villain_position];
		break;
	case VILLAIN_ACTION_RAISE: 
		villain_range = g_raise_villain_preflop_range;
		break;
	default:
		throw poker_exception_t("get_villain_preflop_range: invalid last_villain_action");
	}
	return copy_villain_range(villain_range);
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


bool
has_gutshot(const hand_t* hand, const board_t* board)
{
	std::vector<rank_t> ranks;
	ranks.emplace_back(hand->cards[0]->rank);
	ranks.emplace_back(hand->cards[1]->rank);
	for (const card_t* card : board->cards)
	{
		ranks.emplace_back(card->rank);
	}
	insertion_sort(&ranks[0], ranks.size(),
		[](const rank_t& lhs, const rank_t& rhs)
		{
			return lhs < rhs;
		});
	int consec_before_gap = 0, consec_after_gap = 1;
	for (int i = 0; i < (int)ranks.size() - 1; i++)
	{
		if ((int)ranks[i + 1] == (int)ranks[i] + 1)
		{
			consec_after_gap++;
		}
		else if ((int)ranks[i + 1] == (int)ranks[i] + 2)
		{
			consec_before_gap = consec_after_gap;
			consec_after_gap = 1;
		}
		else
		{
			consec_after_gap = 1;
			consec_before_gap = 0;
		}
		if (consec_after_gap + consec_before_gap == 4)
		{
			return true;
		}
	}
	return false;
}


bool
has_open_ender(const hand_t* hand, const board_t* board)
{
	std::vector<rank_t> ranks;
	ranks.emplace_back(hand->cards[0]->rank);
	ranks.emplace_back(hand->cards[1]->rank);
	for (const card_t* card : board->cards)
	{
		ranks.emplace_back(card->rank);
	}
	insertion_sort(&ranks[0], ranks.size(),
		[](const rank_t& lhs, const rank_t& rhs)
		{
			return lhs < rhs;
		});
	int consec = 1;
	for (int i = 0; i < (int)ranks.size() - 1; i++)
	{
		if ((int)ranks[i + 1] == (int)ranks[i] + 1)
		{
			consec++;
		}
		else
		{
			consec = 1;
		}
		if (consec == 4)
		{
			return true;
		}
	}
	return false;
}


bool
has_flush_draw(const hand_t* hand, const board_t* board)
{
	std::vector<color_t> colors;
	colors.emplace_back(hand->cards[0]->color);
	colors.emplace_back(hand->cards[1]->color);
	for (const card_t* card : board->cards)
	{
		colors.emplace_back(card->color);
	}
	insertion_sort(&colors[0], colors.size(),
		[](const color_t& lhs, const color_t& rhs)
		{
			return lhs < rhs;
		});
	int consec = 1;
	for (int i = 0; i < (int)colors.size() - 1; i++)
	{
		if (colors[i + 1] == colors[i])
		{
			consec++;
		}
		else
		{
			consec = 1;
		}
		if (consec == 4)
		{
			return true;
		}
	}
	return false;
}
