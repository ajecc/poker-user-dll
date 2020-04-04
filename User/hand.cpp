#include "hand.h"
#include "hand_board_result.h"
#include "poker_exception.h"
#include "util.h"
#include "debug.h"
#include <cassert>
#include <algorithm>
#include <iterator>
#include <future>
#include <mutex>

static std::mutex g_mutex;

extern std::vector<card_t*> g_all_cards;

extern std::vector<hand_t*> g_all_hands;

static float calc_prwin_vs_hand_river(hand_t* hero, hand_t* villain, board_t* board);

static float calc_prwin_vs_hand_turn(hand_t* hero, hand_t* villain, board_t* board);

static float calc_prwin_vs_hand_flop(hand_t* hero, hand_t* villain, board_t* board);

static std::vector<card_t*> find_remaining_cards(hand_t* hero, hand_t* villain, board_t* board);


hand_action_t get_hand_action_from_char(char hand_action_char)
{
	hand_action_char = tolower(hand_action_char);
	if (hand_action_char == 'f')
	{
		return FOLD;
	}
	if (hand_action_char == 'c')
	{
		return CALL;
	}
	if (hand_action_char == 'r')
	{
		return RAISE;
	}
	std::string err_msg = "get_hand_action_from_char: invalid char: ";
	err_msg.push_back(hand_action_char);
	throw poker_exception_t(err_msg);
}


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
			all_hands.emplace_back(current_hand);
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
	assert(first_card_ind != second_card_ind);
	if (second_card_ind < first_card_ind)
	{
		std::swap(second_card_ind, first_card_ind);
	}
	size_t ind = first_card_ind * (g_all_cards.size() - 1) -
		(first_card_ind - 1) * first_card_ind / 2 +
		second_card_ind - first_card_ind - 1;
	return g_all_hands[ind];
}


bool is_hero_winner(hand_t* hero, hand_t* villain, board_t* board)
{
	assert(board->cards.size() == 5);
	auto hero_result = calc_hand_board_result(hero, board);
	auto villain_result = calc_hand_board_result(villain, board);
	return hero_result > villain_result;
}


float calc_prwin_vs_hand(hand_t* hero, hand_t* villain, board_t* board)
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


float calc_prwin_vs_any_hand(hand_t* hero, board_t* board)
{
	std::vector<card_t*> remaining_cards;
	std::vector<card_t*> current_cards = {hero->cards[0], hero->cards[1]};
	for (auto* card : board->cards)
	{
		current_cards.emplace_back(card);
	}
	for (auto* card : g_all_cards)
	{
		if (!is_in_vector(card, current_cards))
		{
			remaining_cards.emplace_back(card);
		}
	}
	float prwin = 0, count = 0;
	std::vector<std::future<float>> prwin_futures;
	for (size_t i = 0; i < remaining_cards.size(); i++)
	{
		card_t* lhs_card = remaining_cards[i];
		float res = 0;
		for (size_t j = i + 1; j < remaining_cards.size(); j++)
		{
			card_t* rhs_card = remaining_cards[j];
			hand_t* hand = get_hand(lhs_card, rhs_card);
			prwin_futures.push_back(std::async(std::launch::async, calc_prwin_vs_hand, hero, hand, board));
		}
		count += remaining_cards.size() - i - 1;
	}
	for (auto& prwin_future : prwin_futures)
	{
		prwin += prwin_future.get();
	}
	return prwin / count;
}


bool are_similar_hands(hand_t* lhs, hand_t* rhs)
{
	// TODO: add support for when cards in hand are not in ascending order. for ex: AKs should be equal to KAs
	return lhs->cards[0]->rank == rhs->cards[0]->rank && 
		lhs->cards[1]->rank == rhs->cards[1]->rank && lhs->suited == rhs->suited;
}


static float calc_prwin_vs_hand_river(hand_t* hero, hand_t* villain, board_t* board)
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


static float calc_prwin_vs_hand_turn(hand_t* hero, hand_t* villain, board_t* board)
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
	return (float)hero_wins / (float)remaining_cards.size();
}


static float calc_prwin_vs_hand_flop(hand_t* hero, hand_t* villain, board_t* board)
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
	return (float)hero_wins / (float)(remaining_cards_size * (remaining_cards_size - 1) / 2);
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
		current_cards.emplace_back(card);
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
