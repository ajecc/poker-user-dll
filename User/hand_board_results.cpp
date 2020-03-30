#include "hand_board_result.h"
#include "poker_exception.h"
#include "util.h"
#include <cassert>
#include <algorithm>


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
	if (strength != other.strength)
	{
		return strength < other.strength;
	}
	for (size_t i = 0; i < kickers.size(); i++)
	{
		if (kickers[i]->rank != other.kickers[i]->rank)
		{
			return kickers[i]->rank < other.kickers[i]->rank;
		}
	}
	return false;
}


bool hand_board_result_t::operator>(const hand_board_result_t& other) const
{
	if (strength != other.strength)
	{
		return strength > other.strength;
	}
	for (size_t i = 0; i < kickers.size(); i++)
	{
		if (kickers[i]->rank != other.kickers[i]->rank)
		{
			return kickers[i]->rank > other.kickers[i]->rank;
		}
	}
	return false;
}


bool hand_board_result_t::operator==(const hand_board_result_t& other) const
{
	if (strength != other.strength)
	{
		return false;
	}
	for (size_t i = 0; i < kickers.size(); i++)
	{
		if (kickers[i]->rank != other.kickers[i]->rank)
		{
			return false;
		}
	}
	return true;
}


bool hand_board_result_t::operator<=(const hand_board_result_t& other) const
{
	return !(*this > other);
}


bool hand_board_result_t::operator>=(const hand_board_result_t& other) const
{
	return !(*this < other);
}


bool hand_board_result_t::operator!=(const hand_board_result_t& other) const
{
	return !(*this == other);
}


hand_board_result_t calc_hand_board_result(hand_t* hand, board_t* board)
{
	std::vector<card_t*> cards;
	for (auto* card : board->cards)
	{
		cards.emplace_back(card);
	}
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
	hand_board_result_t result;
	if (calc_straight_flush(cards, &result))
	{
		return result;
	}
	if (calc_quads(cards, &result))
	{
		return result;
	}
	if (calc_full_house(cards, &result))
	{
		return result;
	}
	if (calc_flush(cards, &result))
	{
		return result;
	}
	if (calc_straight(cards, &result))
	{
		return result;
	}
	if (calc_trips(cards, &result))
	{
		return result;
	}
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
	card_t* kicker;
	bool have_result = false;
	for (int i = (int)cards.size() - 2; i >= 0; i--)
	{
		if (cards[i]->rank == cards[i + 1]->rank + 1
			&& cards[i]->color == cards[i + 1]->color)
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
	if (have_result)
	{
		result->strength = STRAIGHT_FLUSH;
		result->kickers.emplace_back(kicker);
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
			result->kickers.emplace_back(cards[i]);
			if (i == 0)
			{
				result->kickers.emplace_back(cards[4]);
			}
			else
			{
				result->kickers.emplace_back(cards[0]);
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
			result->kickers.emplace_back(cards[i]);
			if (i == 0)
			{
				result->kickers.emplace_back(cards[3]);
				result->kickers.emplace_back(cards[4]);
			}
			else
			{
				result->kickers.emplace_back(cards[0]);
				if (i == 1)
				{
					result->kickers.emplace_back(cards[4]);
				}
				else
				{
					result->kickers.emplace_back(cards[1]);
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
			result->kickers.emplace_back(cards[i]);
			for (int j = 0; j < (int)cards.size(); j++)
			{
				if (j != i && j != i + 1)
				{
					result->kickers.emplace_back(cards[j]);
					if (result->kickers.size() == 4)
					{
						break;
					}
				}
			}
			return true;
		}
	}
	return false;
}


static bool calc_full_house(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	hand_board_result_t trips_result; 
	bool have_result = calc_trips(cards, &trips_result);
	if (!have_result)
	{
		return false;
	}
	std::vector<card_t*> new_cards;
	for (auto& card : cards)
	{
		if (card->rank != trips_result.kickers[0]->rank)
		{
			new_cards.emplace_back(card);
		}
	}
	hand_board_result_t pair_result; 
	have_result = calc_pair(new_cards, &pair_result);
	if (!have_result)
	{
		return false;
	}
	result->strength = FULL_HOUSE;
	result->kickers.emplace_back(trips_result.kickers[0]);
	result->kickers.emplace_back(pair_result.kickers[0]);
	return true;
}


static bool calc_two_pair(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	hand_board_result_t pair_result_1;
	bool have_result = calc_pair(cards, &pair_result_1);
	if (!have_result)
	{
		return false;
	}
	std::vector<card_t*> new_cards;
	for (auto& card : cards)
	{
		if (card->rank != pair_result_1.kickers[0]->rank)
		{
			new_cards.emplace_back(card);
		}
	}
	hand_board_result_t pair_result_2;
	have_result = calc_pair(new_cards, &pair_result_2);
	if (!have_result)
	{
		return false;
	}
	result->strength = TWO_PAIR;
	card_t* kicker_1 = pair_result_1.kickers[0];
	card_t* kicker_2 = pair_result_2.kickers[0];
	if (kicker_1->rank < kicker_2->rank)
	{
		std::swap(kicker_1, kicker_2);
	}
	result->kickers.emplace_back(kicker_1);
	result->kickers.emplace_back(kicker_2);
	result->kickers.emplace_back(pair_result_2.kickers[1]);
	return result;
}


static bool calc_high_card(const std::vector<card_t*>& cards, hand_board_result_t* result)
{
	result->strength = HIGH_CARD;
	for (size_t i = 0; i < std::min((size_t)5, cards.size()); i++)
	{
		result->kickers.emplace_back(cards[i]);
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
	card_t* kicker;
	bool have_result = false;
	for (int i = (int)cards_normalized.size() - 2; i >= 0; i--)
	{
		if (cards_normalized[i]->rank == cards_normalized[i + 1]->rank + 1)
		{
			consec++;
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
	if (have_result)
	{
		result->strength = STRAIGHT;
		result->kickers.emplace_back(kicker);
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
	card_t* kicker;
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
	if (have_result)
	{
		result->strength = FLUSH;
		result->kickers.emplace_back(kicker);
	}
	return have_result;
}
