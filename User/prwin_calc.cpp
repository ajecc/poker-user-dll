#include "prwin_calc.h"
#include "util.h"
#include "poker_exception.h"
#include "hand_board_result.h"
#include "board_derived_info.h"
#include "villain_range.h"
#include <filesystem>
#include <cassert>
#include <future>
#include <iostream>
#include <future>


extern std::vector<const card_t*> g_all_cards;

extern std::vector<float> g_all_prwin_vs_any_hand_flop;


static float
calc_prwin_vs_hand_river(const hand_t* hero, const hand_t* villain, const board_t* board);

static float
calc_prwin_vs_hand_turn(const hand_t* hero, const hand_t* villain, const board_t* board);

static float
calc_prwin_vs_hand_flop(const hand_t* hero, const hand_t* villain, const board_t* board);

static float
calc_prwin_vs_any_hand_flop(const hand_t* hero, const board_t* board);


static float
calc_hand_calling_rate(const hand_t* hand, const board_t* board, const bet_type_t& bet_type);


bool
is_hero_winner(const hand_t* hero, const hand_t* villain, const board_t* board)
{
	assert(board->cards.size() == 5);
	auto hero_result = calc_hand_board_result(hero, board);
	auto villain_result = calc_hand_board_result(villain, board);
	return hero_result >= villain_result;
}


float
calc_prwin_vs_hand(const hand_t* hero, const hand_t* villain, const board_t* board)
{
	if(board->cards.size() == 3)
	{
		return calc_prwin_vs_hand_flop(hero, villain, board);
	}
	else if (board->cards.size() == 4)
	{
		return calc_prwin_vs_hand_turn(hero, villain, board);
	}
	else if (board->cards.size() == 5)
	{
		return calc_prwin_vs_hand_river(hero, villain, board);
	}
	throw poker_exception_t("calc_prwin_vs_hand: can't calculate prwin for an empty/invalid board");
}


float
calc_prwin_vs_any_hand(const hand_t* hero, const board_t* board)
{
	if (board->cards.size() == 3)
	{
		return calc_prwin_vs_any_hand_flop(hero, board);
	}
	std::vector<const card_t*> remaining_cards = find_remaining_cards(hero, nullptr, board);
	float prwin = 0, count = 0;
	for (size_t i = 0; i < remaining_cards.size(); i++)
	{
		const card_t* lhs_card = remaining_cards[i];
		for (size_t j = i + 1; j < remaining_cards.size(); j++)
		{
			const card_t* rhs_card = remaining_cards[j];
			const hand_t* hand = get_hand(lhs_card, rhs_card);
			prwin += calc_prwin_vs_hand(hero, hand, board);
			count++;
		}
	}
	return prwin / count;
}


float 
calc_prwin_vs_any_hand_with_draws(const hand_t* hero, const board_t* board,
									const std::vector<const card_t*>& draws)
{
	board_t board_cpy;
	board_cpy.cards = board->cards;
	std::vector<std::future<float>> prwin_futures;
	for (auto* card : draws)
	{
		board_cpy.cards.push_back(card);
		prwin_futures.push_back(
			std::async(std::launch::async, calc_prwin_vs_any_hand, hero, &board_cpy));
		board_cpy.cards.pop_back();
	}
	float prwin = 0;
	for (auto& prwin_future : prwin_futures)
	{
		prwin += prwin_future.get();
	}
	return prwin / prwin_futures.size();
}


std::vector<float>
create_all_prwin_vs_any_hand_flop()
{
	if (!std::filesystem::exists(ALL_PRWIN_VS_ANY_HAND_FLOP_FILE_NAME))
	{
		throw std::runtime_error("create_all_prwin_vs_any_hand: cache file is missing");
	}
	const auto* cache = (const float*)open_cache(ALL_PRWIN_VS_ANY_HAND_FLOP_FILE_NAME,
		ALL_PRWIN_VS_ANY_HAND_FLOP_FILE_NAME);
	const size_t cache_size = (size_t)std::filesystem::file_size(ALL_PRWIN_VS_ANY_HAND_FLOP_FILE_NAME) /
																					sizeof(float);
	return std::vector<float>(cache, cache + cache_size);
}


float
calc_prwin_vs_villain_range(const hand_t* hero, const villain_range_t* villain_range, const board_t* board)
{
	std::vector<std::future<float>> prwin_futures;
	for (const auto* hand : villain_range->villain_range)
	{
		prwin_futures.push_back(
			std::async(std::launch::async, calc_prwin_vs_hand, hero, hand, board)
		);
	}
	float prwin_sum = 0;
	for (auto& prwin_future : prwin_futures)
	{
		prwin_sum += prwin_future.get();
	}
	if (prwin_futures.size() == 0)
	{
		return 1;
	}
	return prwin_sum / (float)prwin_futures.size();
}


float
calc_calling_rate_vs_villain_range(const hand_t* hero, const villain_range_t* villain_range,
	const board_t* board, const bet_type_t& bet_type)
{
	if (villain_range->villain_range.size() == 0)
	{
		return 0;
	}
	float calling_rate = 0;
	for (const hand_t* hand : villain_range->villain_range)
	{
		float calling_rate_local = calc_hand_calling_rate(hand, board, bet_type);
		float prwin = calc_prwin_vs_hand(hero, hand, board);
		calling_rate += calling_rate_local * prwin;
	}
	return calling_rate / (float)villain_range->villain_range.size();
}


static float 
calc_prwin_vs_hand_river(const hand_t* hero, const hand_t* villain, const board_t* board)
{
	if (is_hero_winner(hero, villain, board))
	{
		return 1;
	}
	return 0;
}


static float
calc_prwin_vs_hand_turn(const hand_t* hero, const hand_t* villain, const board_t* board)
{
	board_t board_cpy;
	board_cpy.cards = board->cards;
	auto remaining_cards = find_remaining_cards(hero, villain, &board_cpy);
	int hero_wins = 0;
	for (auto* card : remaining_cards)
	{
		board_cpy.cards.push_back(card);
		if (is_hero_winner(hero, villain, &board_cpy))
		{
			hero_wins++;
		}
		board_cpy.cards.pop_back();
	}
	assert(remaining_cards.size() != 0);
	return (float)hero_wins / (float)remaining_cards.size();
}


static float
calc_prwin_vs_hand_flop(const hand_t* hero, const hand_t* villain, const board_t* board)
{
	board_t board_cpy;
	board_cpy.cards = board->cards;
	auto remaining_cards = find_remaining_cards(hero, villain, &board_cpy);
	int hero_wins = 0;
	auto remaining_cards_size = remaining_cards.size();
	for (size_t i = 0; i < remaining_cards_size; i++)
	{
		board_cpy.cards.push_back(remaining_cards[i]);
		for (size_t j = i + 1; j < remaining_cards_size; j++)
		{
			board_cpy.cards.push_back(remaining_cards[j]);
			if (is_hero_winner(hero, villain, &board_cpy))
			{
				hero_wins++;
			}
			board_cpy.cards.pop_back();
		}
		board_cpy.cards.pop_back();
	}
	return (float)hero_wins / 
		(float)(remaining_cards_size * (remaining_cards_size - 1) / 2);
}


static float 
calc_prwin_vs_any_hand_flop(const hand_t* hero, const board_t* board)
{
	assert(board->cards.size() == 3);
	assert(std::is_sorted(all(board->cards), card_ptr_cmp));
	static constexpr auto comb = comb_t<NUMBER_OF_CARDS, 4>();
	size_t hero_0_ind = get_card_index(hero->cards[0]);
	size_t hero_1_ind = get_card_index(hero->cards[1]);
	if (hero_1_ind < hero_0_ind)
	{
		std::swap(hero_0_ind, hero_1_ind);
	}
	size_t ind = get_hand_index(hero) * comb.comb[NUMBER_OF_CARDS - 2][3];
	size_t card_0_ind = get_card_index(board->cards[0]);
	for (size_t i = 0; i < card_0_ind; i++)
	{
		if (i != hero_0_ind && i != hero_1_ind)
		{
			ind += comb.comb[NUMBER_OF_CARDS - 1 - i][2];
			if (hero_0_ind > i)
			{
				ind -= NUMBER_OF_CARDS - 2 - i;
			}
			if (hero_1_ind > i)
			{
				ind -= NUMBER_OF_CARDS - 2 - i;
			}
			if (hero_0_ind > i && hero_1_ind > i)
			{
				ind++;
			}
		}
	}
	size_t card_1_ind = get_card_index(board->cards[1]);
	for (size_t i = card_0_ind + 1; i < card_1_ind; i++)
	{
		if (i != hero_0_ind && i != hero_1_ind)
		{
			ind += NUMBER_OF_CARDS - 1 - i;
			if (hero_0_ind > i)
			{
				ind--;
			}
			if (hero_1_ind > i)
			{
				ind--;
			}
		}
	}
	size_t card_2_ind = get_card_index(board->cards[2]);
	for (size_t i = card_1_ind + 1; i < card_2_ind; i++)
	{
		if (i != hero_0_ind && i != hero_1_ind)
		{
			ind++;
		}
	}
	assert(ind < g_all_prwin_vs_any_hand_flop.size());
	return g_all_prwin_vs_any_hand_flop[ind];
}


static float
calc_hand_calling_rate(const hand_t* hand, const board_t* board, const bet_type_t& bet_type)
{
	// TODO: add calling rate based on previous actions (i.e reraise => smaller calling rate)
	bool has_gutshot_ = has_gutshot(hand, board);
	bool has_open_ender_ = has_open_ender(hand, board);
	bool has_flush_draw_ = has_flush_draw(hand, board);
	float prwin_vs_any_hand_ = (float)pow(calc_prwin_vs_any_hand(hand, board),
		board->current_hand_players.size() - 1);
	if (bet_type == OPEN)
	{
		if (board->stage == FLOP)
		{
			if (has_flush_draw_)
			{
				return 1.0f;
			}
			if (prwin_vs_any_hand_ > 0.71f)
			{
				return 1.0f;
			}
			if (prwin_vs_any_hand_ > 0.65f)
			{
				return 0.90f;
			}
			if (has_open_ender_)
			{
				return 0.8f;
			}
			if (prwin_vs_any_hand_ > 0.58f)
			{
				return 0.6f;
			}
			if (has_gutshot_ && prwin_vs_any_hand_ > 0.4f)
			{
				return 0.3f;
			}
			if (prwin_vs_any_hand_ > 0.52f)
			{
				return 0.3f;
			}
			if (has_gutshot_)
			{
				return 0.2f;
			}
			return 0.05f;
		}
		else if (board->stage == TURN)
		{
			if (prwin_vs_any_hand_ > 0.76f)
			{
				return 1.0f;
			}
			if (prwin_vs_any_hand_ > 0.74f)
			{
				return 0.9f;
			}
			if (has_flush_draw_ && has_open_ender_)
			{
				return 0.8f;
			}
			if (has_flush_draw_ && prwin_vs_any_hand_ > 0.68f)
			{
				return 0.6f;
			}
			if (has_flush_draw_)
			{
				return 0.5f;
			}
			if (has_open_ender_ && prwin_vs_any_hand_ > 0.63f)
			{
				return 0.5f;
			}
			if (prwin_vs_any_hand_ > 0.47f)
			{
				return 0.5f;
			}
			if (has_gutshot_)
			{
				return 0.1f;
			}
			return 0.0f;
		}
		else if (board->stage == RIVER)
		{
			if (prwin_vs_any_hand_ > 0.87f)
			{
				return 1.0f;
			}
			if (prwin_vs_any_hand_ > 0.84f)
			{
				return 0.9f;
			}
			if (prwin_vs_any_hand_ > 0.8f)
			{
				return 0.85f;
			}
			if (prwin_vs_any_hand_ > 0.73f)
			{
				return 0.5f;
			}
			if (prwin_vs_any_hand_ > 0.53f)
			{
				return 0.3f;
			}
			return 0.0f;
		}
		throw poker_exception_t("calc_hand_would_call_percent: invalid board stage");
	}
	else
	{
		if (prwin_vs_any_hand_ > 0.88f)
		{
			return 1.0f;
		}
		if (prwin_vs_any_hand_ > 0.84f)
		{
			return 0.9f;
		}
		if (has_flush_draw_ && has_open_ender_ && board->stage == FLOP)
		{
			return 0.9f;
		}
		if (prwin_vs_any_hand_ > 0.78f)
		{
			return 0.7f;
		}
		if (has_flush_draw_ && has_open_ender_ && board->stage == TURN)
		{
			return 0.4f;
		}
		if (has_flush_draw_ && board->stage == FLOP)
		{
			return 0.3f;
		}
		return 0.0f;
	}
}
