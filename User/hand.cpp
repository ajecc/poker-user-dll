#include "hand.h"
#include "hand_board_result.h"
#include "poker_exception.h"
#include "util.h"
#include "debug.h"
#include "board_derived_info.h"
#include <cassert>
#include <algorithm>
#include <future>


extern std::vector<const card_t*> g_all_cards;

extern std::vector<const hand_t*> g_all_hands;


hand_action_t
get_hand_action_from_char(char hand_action_char)
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


bool 
hand_t::operator<(const hand_t& other) const
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


std::vector<const hand_t*> 
create_all_hands()
{
	std::vector<const hand_t*> all_hands;
	for (size_t i = 0; i < g_all_cards.size(); i++)
	{
		for (size_t j = i + 1; j < g_all_cards.size(); j++)
		{
			auto* current_hand = new hand_t;
			current_hand->cards[0] = g_all_cards[i];
			current_hand->cards[1] = g_all_cards[j];
			current_hand->suited = 
				(current_hand->cards[0]->color == current_hand->cards[1]->color);
			all_hands.emplace_back(current_hand);
		}
	}
	assert(std::is_sorted(all(all_hands), [](const hand_t* lhs, const hand_t* rhs)
		{
			return *lhs < *rhs;
		}
	));
	return all_hands;
}


size_t
get_hand_index(const card_t* card_0, const card_t* card_1)
{
	int first_card_ind = ((int)card_0->rank - 2) * 4 + (int)card_0->color;
	int second_card_ind = ((int)card_1->rank - 2) * 4 + (int)card_1->color;
	assert(first_card_ind != second_card_ind);
	if (second_card_ind < first_card_ind)
	{
		std::swap(second_card_ind, first_card_ind);
	}
	return first_card_ind * (g_all_cards.size() - 1) -
		(first_card_ind - 1) * first_card_ind / 2 +
		second_card_ind - first_card_ind - 1;
}


size_t
get_hand_index(const hand_t* hand)
{
	return get_hand_index(hand->cards[0], hand->cards[1]);
}


const hand_t* 
get_hand(const card_t* card_0, const card_t* card_1)
{
	return g_all_hands[get_hand_index(card_0, card_1)];
}


bool 
are_similar_hands(const hand_t* lhs, const hand_t* rhs)
{
	// NOTE: only works for hands found in g_all_hands
	return lhs->cards[0]->rank == rhs->cards[0]->rank && 
		lhs->cards[1]->rank == rhs->cards[1]->rank && lhs->suited == rhs->suited;
}


std::string
hand_t::to_string() const
{
	return cards[0]->to_string() + " " + cards[1]->to_string();
}
