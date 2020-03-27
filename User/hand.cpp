#include "hand.h"
#include "hand_board_result.h"
#include "poker_exception.h"
#include "util.h"
#include <cassert>
#include <algorithm>
#include <iterator>

extern std::vector<card_t> g_all_cards;

static double calc_prwin_vs_hand_river(hand_t* hero, hand_t* villain, board_t* board);

static double calc_prwin_vs_hand_turn(hand_t* hero, hand_t* villain, board_t* board);

static double calc_prwin_vs_hand_flop(hand_t* hero, hand_t* villain, board_t* board);

static std::vector<card_t> find_remaining_cards(hand_t* hero, hand_t* villain, board_t* board);


bool is_hero_winner(hand_t* hero, hand_t* villain, board_t* board)
{
	assert(board->cards.size() == 5);
	auto hero_result = calc_hand_board_result(hero, board);
	auto villain_result = calc_hand_board_result(villain, board);
	return hero_result > villain_result;
}


double calc_prwin_vs_hand(hand_t* hero, hand_t* villain, board_t* board)
{
	if (board->cards.size() == 5)
	{
		return calc_prwin_vs_hand_river(hero, villain, board);
	}
	else if (board->cards.size() == 4)
	{
		return calc_prwin_vs_hand_turn(hero, villain, board);
	}
	else if(board->cards.size() == 3)
	{
		return calc_prwin_vs_hand_flop(hero, villain, board);
	}
	throw poker_exception_t("calc_prwin_vs_hand: can't calculate prwin for an empty/invalid board");
}


static double calc_prwin_vs_hand_river(hand_t* hero, hand_t* villain, board_t* board)
{
	if (is_hero_winner(hero, villain, board))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


static double calc_prwin_vs_hand_turn(hand_t* hero, hand_t* villain, board_t* board)
{
	auto remaining_cards = find_remaining_cards(hero, villain, board);
	int hero_wins = 0;
	for (auto& card : remaining_cards)
	{
		board->cards.push_back(&card);
		if (is_hero_winner(hero, villain, board))
		{
			hero_wins++;
		}
		board->cards.pop_back();
	}
	return (double)hero_wins / (double)remaining_cards.size();
}


static double calc_prwin_vs_hand_flop(hand_t* hero, hand_t* villain, board_t* board)
{
	auto remaining_cards = find_remaining_cards(hero, villain, board);
	int hero_wins = 0;
	auto remaining_cards_size = remaining_cards.size();
	for (size_t i = 0; i < remaining_cards_size; i++)
	{
		board->cards.push_back(&remaining_cards[i]);
		for (size_t j = i + 1; j < remaining_cards_size; j++)
		{
			board->cards.push_back(&remaining_cards[j]);
			if (is_hero_winner(hero, villain, board))
			{
				hero_wins++;
			}
			board->cards.pop_back();
		}
		board->cards.pop_back();
	}
	return (double)hero_wins / (double)(remaining_cards_size * (remaining_cards_size - 1) / 2);
}


static std::vector<card_t> find_remaining_cards(hand_t* hero, hand_t* villain, board_t* board)
{
	std::vector<card_t> current_cards;
	current_cards.emplace_back(hero->cards[0]);
	current_cards.emplace_back(hero->cards[1]);
	current_cards.emplace_back(villain->cards[0]);
	current_cards.emplace_back(villain->cards[1]);
	for (auto* card : board->cards)
	{
		current_cards.emplace_back(*card);
	}
	std::vector<card_t> remaining_cards;
	std::set_difference(
		all(g_all_cards),
		all(current_cards),
		std::inserter(remaining_cards, remaining_cards.begin())
	);
	return remaining_cards;
}
