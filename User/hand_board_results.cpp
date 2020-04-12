#include "hand_board_result.h"
#include "poker_exception.h"
#include "util.h"
#include <cassert>
#include <algorithm>


// NOTE: result should be init with 0 before calling these 

static bool calc_straight_flush(std::vector<card_t*> cards, hand_board_result_t* result);

static bool calc_quads(const std::vector<card_t*>& cards, hand_board_result_t* result);

static bool calc_full_house(const std::vector<card_t*>& cards, hand_board_result_t* result);

static bool calc_flush(std::vector<card_t*> cards, hand_board_result_t* result);

static bool calc_straight(const std::vector<card_t*>& cards, hand_board_result_t* result);

static bool calc_trips(const std::vector<card_t*>& cards, hand_board_result_t* result);

static bool calc_two_pair(const std::vector<card_t*>& cards, hand_board_result_t* result);

static bool calc_pair(const std::vector<card_t*>& cards, hand_board_result_t* result);

static bool calc_high_card(const std::vector<card_t*>& cards, hand_board_result_t* result);



bool hand_board_result_t::operator<(const hand_board_result_t& other) const
{
	return *(unsigned int*)this < *(unsigned int*)&other;
}


bool hand_board_result_t::operator>(const hand_board_result_t& other) const
{
	return *(unsigned int*)this > *(unsigned int*)&other;
}


bool hand_board_result_t::operator==(const hand_board_result_t& other) const
{
	return *(unsigned int*)this == *(unsigned int*)&other;
}


bool hand_board_result_t::operator<=(const hand_board_result_t& other) const
{
	return *(unsigned int*)this <= *(unsigned int*)&other;
}


bool hand_board_result_t::operator>=(const hand_board_result_t& other) const
{
	return *(unsigned int*)this >= *(unsigned int*)&other;
}


bool hand_board_result_t::operator!=(const hand_board_result_t& other) const
{
	return *(unsigned int*)this != *(unsigned int*)&other;
}


hand_board_result_t calc_hand_board_result_uncached(hand_t* hand, board_t* board)
{
	std::vector<card_t*> cards = board->cards;
	assert(cards.size() == 5);
	cards.emplace_back(hand->cards[0]);
	cards.emplace_back(hand->cards[1]);
	std::sort(all(cards), [](card_t* lhs, card_t* rhs)
		{
			return *lhs > * rhs;
		});
#ifdef _DEBUG
	for (int i = 1; i < (int)cards.size(); i++)
	{
		if (*(cards[i]) == *(cards[i - 1]))
		{
			throw poker_exception_t("calc_hand_board_result: duplicates in hand/board ("  +
			cards[i]->to_string() + ", " + cards[i - 1]->to_string() + ")");
		}
	}
#endif
	// TODO: rewrite this entierly
	hand_board_result_t result = { 0 };
	if (calc_straight_flush(cards, &result))
	{
		return result;
	}
	if (calc_quads(cards, &result))
	{
		return result;
	}
	// has some impact
	if (calc_full_house(cards, &result))
	{
		return result;
	}
	if (calc_flush(cards, &result))
	{
		return result;
	}
	// has some impact
	if (calc_straight(cards, &result))
	{
		return result;
	}
	if (calc_trips(cards, &result))
	{
		return result;
	}
	// has a lot of impact on performance
	if (calc_two_pair(cards, &result))
	{
		return result;
	}
	if (calc_pair(cards, &result))
	{
		return result;
	}
	if (calc_high_card(cards, &result))
	{
		return result;
	}
	throw poker_exception_t("calc_hand_board_result: calc_high_card should return true");
}


static bool calc_straight_flush(std::vector<card_t*> cards, hand_board_result_t* result)
{
	card_t small_aces[] = { {H, _A_1}, {D, _A_1}, {C, _A_1}, {S, _A_1} };
	for (int i = 0; ; i++)
	{
		if (cards[i]->rank == _A)
		{
			cards.push_back(&small_aces[(int)cards[i]->color]);
		}
		else
		{
			break;
		}
	}
	std::sort(all(cards), [](card_t* lhs, card_t* rhs)
		{
			if (lhs->color == rhs->color)
			{
				return lhs->rank > rhs->rank;
			}
			return lhs->color < rhs->color;
		});
	int consec = 1;
	card_t* kicker = nullptr;
	bool have_result = false;
	for (int i = (int)cards.size() - 2; i >= 0; i--)
	{
		if (cards[i]->rank == cards[i + 1]->rank + 1
			&& cards[i]->color == cards[i + 1]->color)
		{
			consec++;
		}
		else if (cards[i]->rank == cards[i + 1]->rank)
		{
			continue;
		}
		else
		{
			consec = 1;
		}
		if (consec >= 5)
		{
			kicker = cards[i];
			have_result = true;
		}
	}
	if (have_result && kicker != nullptr)
	{
		result->strength = STRAIGHT_FLUSH;
		result->kicker_0 = kicker->rank;
	}
	return have_result;
}


static bool calc_quads(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	for (int i = 0; i < (int)cards.size() - 3; i++)
	{
		if (cards[i]->rank == cards[i + 1]->rank && cards[i]->rank == cards[i + 2]->rank &&
			cards[i]->rank == cards[i + 3]->rank)
		{
			result->strength = QUADS;
			result->kicker_0 = cards[i]->rank;
			if (i == 0)
			{
				result->kicker_1 = cards[4]->rank;
			}
			else
			{
				result->kicker_1 = cards[0]->rank;
			}
			return true;
		}
	}
	return false;
}


static bool calc_trips(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	for (int i = 0; i < (int)cards.size() - 2; i++)
	{
		if (cards[i]->rank == cards[i + 1]->rank && cards[i]->rank == cards[i + 2]->rank)
		{
			result->strength = TRIPS;
			result->kicker_0 = cards[i]->rank;
			if (i == 0)
			{
				result->kicker_1 = cards[3]->rank;
				result->kicker_2 = cards[4]->rank;
			}
			else
			{
				result->kicker_1 = cards[0]->rank;
				if (i == 1)
				{
					result->kicker_2 = cards[4]->rank;
				}
				else
				{
					result->kicker_2 = cards[1]->rank;
				}
			}
			return true;
		}
	}
	return false;
}


static bool calc_pair(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	for (int i = 0; i < (int)cards.size() - 1; i++)
	{
		if (cards[i]->rank == cards[i + 1]->rank)
		{
			result->strength = PAIR;
			result->kicker_0 = cards[i]->rank;
			int kickers_size = 1;
			for (int j = 0; j < (int)cards.size(); j++)
			{
				if (j != i && j != i + 1)
				{
					if (kickers_size == 1)
					{
						result->kicker_1 = cards[j]->rank;
					}
					else if (kickers_size == 2)
					{
						result->kicker_2 = cards[j]->rank;
					}
					else if (kickers_size == 3)
					{
						result->kicker_3 = cards[j]->rank;
					}
					else if (kickers_size == 4)
					{
						break;
					}
					else
					{
						throw poker_exception_t("calc_pair: invalid kicker size");
					}
					kickers_size++;
				}
			}
			return true;
		}
	}
	return false;
}


static bool calc_full_house(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	hand_board_result_t trips_result = { 0 };
	bool have_result = calc_trips(cards, &trips_result);
	if (!have_result)
	{
		return false;
	}
	std::vector<card_t*> new_cards;
	for (auto& card : cards)
	{
		if (card->rank != trips_result.kicker_0)
		{
			new_cards.emplace_back(card);
		}
	}
	hand_board_result_t pair_result = { 0 };
	have_result = calc_pair(new_cards, &pair_result);
	if (!have_result)
	{
		return false;
	}
	result->strength = FULL_HOUSE;
	result->kicker_0 = trips_result.kicker_0;
	result->kicker_1 = pair_result.kicker_0;
	return true;
}


static bool calc_two_pair(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	hand_board_result_t pair_result_1 = { 0 };
	bool have_result = calc_pair(cards, &pair_result_1);
	if (!have_result)
	{
		return false;
	}
	std::vector<card_t*> new_cards;
	for (auto& card : cards)
	{
		if (card->rank != pair_result_1.kicker_0)
		{
			new_cards.emplace_back(card);
		}
	}
	hand_board_result_t pair_result_2 = { 0 };
	have_result = calc_pair(new_cards, &pair_result_2);
	if (!have_result)
	{
		return false;
	}
	result->strength = TWO_PAIR;
	rank_t kicker_1 = pair_result_1.kicker_0;
	rank_t kicker_2 = pair_result_2.kicker_0;
	if (kicker_1 < kicker_2)
	{
		std::swap(kicker_1, kicker_2);
	}
	result->kicker_0 = kicker_1;
	result->kicker_1 = kicker_2;
	result->kicker_2 = pair_result_2.kicker_1;
	return result;
}


static bool calc_high_card(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	result->strength = HIGH_CARD;
	for (size_t i = 0; i < std::min((size_t)5, cards.size()); i++)
	{
		if (i == 0)
		{
			result->kicker_0 = cards[i]->rank;
		}
		else if (i == 1)
		{
			result->kicker_1 = cards[i]->rank;
		}
		else if (i == 2)
		{
			result->kicker_2 = cards[i]->rank;
		}
		else if (i == 3)
		{
			result->kicker_3 = cards[i]->rank;
		}
		else if (i == 4)
		{
			result->kicker_4 = cards[i]->rank;
		}
	}
	return true;
}


static bool calc_straight(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	std::vector<card_t*> cards_normalized;
	for (auto* i : cards)
	{
		if (cards_normalized.empty())
		{
			cards_normalized.push_back(i);
			continue;
		}
		if (cards_normalized.back()->rank != i->rank)
		{
			cards_normalized.push_back(i);
		}
	}
	card_t small_ace = { H, _A_1 };
	if (cards_normalized[0]->rank == _A)
	{
		cards_normalized.push_back(&small_ace);
	}
	int consec = 1;
	card_t* kicker = nullptr;
	bool have_result = false;
	for (int i = (int)cards_normalized.size() - 2; i >= 0; i--)
	{
		if (cards_normalized[i]->rank == cards_normalized[i + 1]->rank + 1)
		{
			consec++;
		}
		else if (cards_normalized[i]->rank == cards_normalized[i + 1]->rank)
		{
			continue;
		}
		else
		{
			consec = 1;
		}
		if (consec >= 5)
		{
			kicker = cards_normalized[i];
			have_result = true;
		}
	}
	if (have_result && kicker != nullptr)
	{
		result->strength = STRAIGHT;
		result->kicker_0 = kicker->rank;
	}
	return have_result;
}


static bool calc_flush(std::vector<card_t*> cards, hand_board_result_t* result)
{
	std::sort(all(cards), [](card_t* lhs, card_t* rhs)
		{
			if (lhs->color == rhs->color)
			{
				return lhs->rank > rhs->rank;
			}
			return lhs->color < rhs->color;
		});
	int consec = 1;
	card_t* kicker = nullptr;
	bool have_result = false;
	for (int i = (int)cards.size() - 2; i >= 0; i--)
	{
		if (cards[i]->color == cards[i + 1]->color)
		{
			consec++;
		}
		else
		{
			consec = 1;
		}
		if (consec >= 5)
		{
			kicker = cards[i];
			have_result = true;
		}
	}
	if (have_result && kicker != nullptr)
	{
		result->strength = FLUSH;
		result->kicker_0 = kicker->rank;
	}
	return have_result;
}
