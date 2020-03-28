#include "hand.h"
#include "hand_board_result.h"
#include "poker_exception.h"
#include "util.h"
#include <cassert>
#include <algorithm>
#include <iterator>

extern std::vector<card_t*> g_all_cards;

extern std::vector<hand_t*> g_all_hands;

static double calc_prwin_vs_hand_river(hand_t* hero, hand_t* villain, board_t* board);

static double calc_prwin_vs_hand_turn(hand_t* hero, hand_t* villain, board_t* board);

static double calc_prwin_vs_hand_flop(hand_t* hero, hand_t* villain, board_t* board);

static std::vector<card_t*> find_remaining_cards(hand_t* hero, hand_t* villain, board_t* board);


bool hand_t::operator<(const hand_t& other) const
{
	for (int i = 0; i < HAND_CARD_COUNT; i++)
	{
		if (*cards[i] != *other.cards[i])
		{
			return *cards[i] < *other.cards[i];
		}
	}
	return false;
}


std::vector<hand_t*> create_all_hands()
{
	std::vector<hand_t*> all_hands;
	for (size_t i = 0; i < g_all_cards.size(); i++)
	{
		for (size_t j = i + 1; j < g_all_cards.size(); j++)
		{
			auto* current_hand = new hand_t;
			current_hand->cards[0] = g_all_cards[i];
			current_hand->cards[1] = g_all_cards[j];
			// NOTE: only works for 2-card hold'em
			current_hand->suited = (current_hand->cards[0]->color == current_hand->cards[1]->color);
		}
	}
	assert(std::is_sorted(all(all_hands), [](hand_t* lhs, hand_t* rhs)
		{
			return *lhs < *rhs;
		}
	));
	return all_hands;
}


hand_t* get_hand(card_t* card_1, card_t* card_2)
{
	// TODO: check this !!!
	int first_card_ind = ((int)card_1->rank - 2) * 4 + (int)card_1->color;
	int second_card_ind = ((int)card_2->rank - 2) * 4 + (int)card_2->color;
	if (second_card_ind > first_card_ind)
	{
		std::swap(second_card_ind, first_card_ind);
	}
	return g_all_hands[
		first_card_ind * g_all_cards.size() -
			(first_card_ind - 1) * first_card_ind / 2 +
			second_card_ind - first_card_ind
	];
}


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
	for (auto* card : remaining_cards)
	{
		board->cards.push_back(card);
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
		board->cards.push_back(remaining_cards[i]);
		for (size_t j = i + 1; j < remaining_cards_size; j++)
		{
			board->cards.push_back(remaining_cards[j]);
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


static std::vector<card_t*> find_remaining_cards(hand_t* hero, hand_t* villain, board_t* board)
{
	std::vector<card_t*> current_cards;
	current_cards.emplace_back(hero->cards[0]);
	current_cards.emplace_back(hero->cards[1]);
	current_cards.emplace_back(villain->cards[0]);
	current_cards.emplace_back(villain->cards[1]);
	for (auto* card : board->cards)
	{
		current_cards.emplace_back(*card);
	}
	std::sort(all(current_cards), [](card_t* lhs, card_t* rhs)
		{
			return *lhs < *rhs;
		});

	std::vector<card_t*> remaining_cards;
	size_t current_cards_ind = 0;
	for (size_t i = 0; i < g_all_cards.size(); i++)
	{
		if (current_cards_ind < current_cards.size() && 
			g_all_cards[i] == current_cards[current_cards_ind])
		{
			current_cards_ind++;
		}
		else
		{
			remaining_cards.emplace_back(g_all_cards[i]);
		}
	}
	return remaining_cards;
}
