#include "hand_board_result.h"
#include "poker_exception.h"
#include "util.h"
#include <cassert>
#include <algorithm>


// TODO: fix some of these, im sure some are broken in edge cases

//static bool calc_straight_flush(std::vector<card_t*> cards, hand_board_result_t* result);
//
//static bool calc_quads(const std::vector<card_t*>& cards, hand_board_result_t* result);
//
//static bool calc_full_house(const std::vector<card_t*>& cards, hand_board_result_t* result);
//
//static bool calc_flush(std::vector<card_t*> cards, hand_board_result_t* result);
//
//static bool calc_straight(const std::vector<card_t*>& cards, hand_board_result_t* result);
//
//static bool calc_trips(const std::vector<card_t*>& cards, hand_board_result_t* result);
//
//static bool calc_two_pair(const std::vector<card_t*>& cards, hand_board_result_t* result);
//
//static bool calc_pair(const std::vector<card_t*>& cards, hand_board_result_t* result);
//
//static bool calc_high_card(const std::vector<card_t*>& cards, hand_board_result_t* result);



bool hand_board_result_t::operator<(const hand_board_result_t& other) const
{
	if (strength != other.strength)
	{
		return strength < other.strength;
	}
	for (size_t i = 0; i < kickers.size(); i++)
	{
		if (kickers[i] != other.kickers[i])
		{
			return kickers[i] < other.kickers[i];
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
		if (kickers[i] != other.kickers[i])
		{
			return kickers[i] > other.kickers[i];
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
		if (kickers[i] != other.kickers[i])
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
	// NOTE: this is a critical secion of the code
	// it might look messy, but it moves fast and quite a bit of effort
	// has been made to here. Do not change to make it more readable
	hand_board_result_t result;
	card_t* cards[7];
	int idx = 0;
	bool inserted_left = false, inserted_right = false;
	auto* left_card = hand->cards[0];
	auto* right_card = hand->cards[1];

	rank_t max_pair_rank = INVALID_RANK, sec_max_pair_rank = INVALID_RANK, current_pair_rank = INVALID_RANK;
	int max_pair_size = 0, sec_max_pair_size = 0, current_pair_size = 0;
	rank_t max_consec_rank = INVALID_RANK, current_consec_rank = INVALID_RANK;
	int max_consec = 0, current_consec = 0;
	bool have_potential_small_straight = false;

	int color_count[COLOR_COUNT] = { 0 };
	int max_color_rank[COLOR_COUNT] = { INVALID_RANK };
	int consec_color = 0;
	rank_t straight_flush = INVALID_RANK;
	rank_t flush = INVALID_RANK;
	bool have_potential_small_straight_flush = false;
	color_t potential_small_straight_color = COLOR_COUNT;

	auto update = [&]()
	{
		if (current_pair_rank != cards[idx]->rank)
		{
			if (max_pair_size <= current_pair_size)
			{
				sec_max_pair_rank = max_pair_rank;
				sec_max_pair_size = max_pair_size;
				max_pair_rank = current_pair_rank;
				max_pair_size = current_pair_size;
			}
			else if (sec_max_pair_size <= current_pair_size)
			{
				sec_max_pair_rank = current_pair_rank;
				sec_max_pair_size = current_pair_size;
			}
			current_pair_rank = cards[idx]->rank;
			current_pair_size = 1;
		}
		else
		{
			current_pair_size++;
		}
		if ((int)cards[idx]->rank == (int)current_consec_rank + 1)
		{
			current_consec_rank = cards[idx]->rank;
			current_consec++;
			if (cards[idx]->rank == _5 && current_consec == 4)
			{
				have_potential_small_straight = true;
				potential_small_straight_color = cards[idx]->color;
			}
		}
		else if (cards[idx]->rank != current_consec_rank)
		{
			if (current_consec >= max_consec)
			{
				max_consec = current_consec;
				max_consec_rank = current_consec_rank;
			}
			current_consec_rank = cards[idx]->rank;
			current_consec = 1;
		}
		if (max_color_rank[(int)cards[idx]->color] + 1 == cards[idx]->rank)
		{
			consec_color++;
			if (consec_color >= 5)
			{
				straight_flush = cards[idx]->rank;
			}
			else if (consec_color == 4 && cards[idx]->rank)
			{
				have_potential_small_straight_flush = true;
			}
		}
		else
		{
			consec_color = 1;
		}
		color_count[(int)cards[idx]->color]++;
		if (color_count[(int)cards[idx]->color] >= 5)
		{
			flush = cards[idx]->rank;
		}
	};
	for(int i = 0; i < (int)board->cards.size(); i++)
	{
		auto* card = board->cards[i];
		if (!inserted_left)
		{
			if (*card < *left_card)
			{
				cards[idx] = card;
			}
			else
			{
				cards[idx] = left_card;
				inserted_left = true;
				i--;
			}
		}
		else if (!inserted_right)
		{
			if (*card < *right_card)
			{
				cards[idx] = card;
			}
			else
			{
				cards[idx] = right_card;
				inserted_right = true;
				i--;
			}
		}
		else
		{
			cards[idx] = card;
		}
		update();
		idx++;
	}
	if (!inserted_left)
	{
		cards[idx] = left_card;
		update();
		idx++;
	}
	if (!inserted_right)
	{
		cards[idx] = right_card;
		update();
	}
	if (straight_flush != INVALID_RANK)
	{
		result.strength = STRAIGHT_FLUSH;
		result.kickers.emplace_back(straight_flush);
		return result;
	}
	if (have_potential_small_straight_flush && max_color_rank[(int)potential_small_straight_color] == _A)
	{
		result.strength = STRAIGHT_FLUSH;
		result.kickers.emplace_back(_A);
		return result;
	}
	if (max_pair_size <= current_pair_size)
	{
		sec_max_pair_rank = max_pair_rank;
		sec_max_pair_size = max_pair_size;
		max_pair_rank = current_pair_rank;
		max_pair_size = current_pair_size;
	}
	else if (sec_max_pair_size <= current_pair_size)
	{
		sec_max_pair_rank = current_pair_rank;
		sec_max_pair_size = current_pair_size;
	}
	if (current_consec >= max_consec)
	{
		max_consec = current_consec;
		max_consec_rank = current_consec_rank;
	}
	if (max_pair_size == 4)
	{
		result.strength = QUADS;
		result.kickers.emplace_back(max_pair_rank);
		if (cards[6]->rank != max_pair_rank)
		{
			result.kickers.emplace_back(cards[6]->rank);
		}
		else
		{
			result.kickers.emplace_back(cards[2]->rank);
		}
		return result;
	}
	if (max_pair_size == 3 && sec_max_pair_size >= 2)
	{
		result.strength = FULL_HOUSE;
		result.kickers.emplace_back(max_pair_rank);
		result.kickers.emplace_back(sec_max_pair_rank);
		return result;
	}
	if (flush != INVALID_RANK)
	{
		result.strength = FLUSH;
		result.kickers.emplace_back(flush);
		return result;
	}
	if (max_consec >= 5)
	{
		result.strength = STRAIGHT;
		result.kickers.emplace_back(max_consec_rank);
		return result;
	}
	if (have_potential_small_straight && cards[6]->rank == _A)
	{
		result.strength = STRAIGHT;
		result.kickers.emplace_back(_5);
		return result;
	}
	if (max_pair_size == 3)
	{
		result.strength = TRIPS;
		result.kickers.emplace_back(max_pair_rank);
		result.kickers.emplace_back(sec_max_pair_rank);
		if (cards[5]->rank != max_pair_rank)
		{
			result.kickers.emplace_back(cards[5]->rank);
		}
		else
		{
			result.kickers.emplace_back(cards[2]->rank);
		}
		return result;
	}
	if (max_pair_size == 2 && sec_max_pair_size == 2)
	{
		result.strength = TWO_PAIR;
		result.kickers.emplace_back(max_pair_rank);
		result.kickers.emplace_back(sec_max_pair_rank);
		if (cards[6]->rank != max_pair_rank)
		{
			result.kickers.emplace_back(cards[6]->rank);
		}
		else if (cards[4]->rank != sec_max_pair_rank)
		{
			result.kickers.emplace_back(cards[4]->rank);
		}
		else
		{
			result.kickers.emplace_back(cards[2]->rank);
		}
		return result;
	}
	if (max_pair_size == 2)
	{
		result.strength = PAIR;
		result.kickers.emplace_back(max_pair_rank);
		result.kickers.emplace_back(sec_max_pair_rank);
		for (int i = 6; i >= 0 && result.kickers.size() < 4; i--)
		{
			if (cards[i]->rank != max_pair_rank && cards[i]->rank != sec_max_pair_rank)
			{
				result.kickers.emplace_back(cards[i]->rank);
			}
		}
		return result;
	}
	result.strength = HIGH_CARD;
	for (int i = 6; i >= 2; i--)
	{
		result.kickers.emplace_back(cards[i]->rank);
	}
	return result;
}


//static bool calc_straight_flush(std::vector<card_t*> cards, hand_board_result_t* result)
//{
//	card_t small_aces[] = { {H, _A_1}, {D, _A_1}, {C, _A_1}, {S, _A_1} };
//	for (int i = 0; ; i++)
//	{
//		if (cards[i]->rank == _A)
//		{
//			cards.push_back(&small_aces[(int)cards[i]->color]);
//		}
//		else
//		{
//			break;
//		}
//	}
//	std::sort(all(cards), [](card_t* lhs, card_t* rhs)
//		{
//			if (lhs->color == rhs->color)
//			{
//				return lhs->rank > rhs->rank;
//			}
//			return lhs->color < rhs->color;
//		});
//	int consec = 1;
//	card_t* kicker;
//	bool have_result = false;
//	for (int i = (int)cards.size() - 2; i >= 0; i--)
//	{
//		if (cards[i]->rank == cards[i + 1]->rank + 1
//			&& cards[i]->color == cards[i + 1]->color)
//		{
//			consec++;
//		}
//		else
//		{
//			consec = 1;
//		}
//		if (consec >= 5)
//		{
//			kicker = cards[i];
//			have_result = true;
//		}
//	}
//	if (have_result)
//	{
//		result->strength = STRAIGHT_FLUSH;
//		result->kickers.emplace_back(kicker);
//	}
//	return have_result;
//}
//
//
//static bool calc_quads(const std::vector<card_t*>& cards, hand_board_result_t* result)
//{
//	for (int i = 0; i < (int)cards.size() - 3; i++)
//	{
//		if (cards[i]->rank == cards[i + 1]->rank && cards[i]->rank == cards[i + 2]->rank &&
//			cards[i]->rank == cards[i + 3]->rank)
//		{
//			result->strength = QUADS;
//			result->kickers.emplace_back(cards[i]);
//			if (i == 0)
//			{
//				result->kickers.emplace_back(cards[4]);
//			}
//			else
//			{
//				result->kickers.emplace_back(cards[0]);
//			}
//			return true;
//		}
//	}
//	return false;
//}
//
//
//static bool calc_trips(const std::vector<card_t*>& cards, hand_board_result_t* result)
//{
//	for (int i = 0; i < (int)cards.size() - 2; i++)
//	{
//		if (cards[i]->rank == cards[i + 1]->rank && cards[i]->rank == cards[i + 2]->rank)
//		{
//			result->strength = TRIPS;
//			result->kickers.emplace_back(cards[i]);
//			if (i == 0)
//			{
//				result->kickers.emplace_back(cards[3]);
//				result->kickers.emplace_back(cards[4]);
//			}
//			else
//			{
//				result->kickers.emplace_back(cards[0]);
//				if (i == 1)
//				{
//					result->kickers.emplace_back(cards[4]);
//				}
//				else
//				{
//					result->kickers.emplace_back(cards[1]);
//				}
//			}
//			return true;
//		}
//	}
//	return false;
//}
//
//
//static bool calc_pair(const std::vector<card_t*>& cards, hand_board_result_t* result)
//{
//	for (int i = 0; i < (int)cards.size() - 1; i++)
//	{
//		if (cards[i]->rank == cards[i + 1]->rank)
//		{
//			result->strength = PAIR;
//			result->kickers.emplace_back(cards[i]);
//			for (int j = 0; j < (int)cards.size(); j++)
//			{
//				if (j != i && j != i + 1)
//				{
//					result->kickers.emplace_back(cards[j]);
//					if (result->kickers.size() == 4)
//					{
//						break;
//					}
//				}
//			}
//			return true;
//		}
//	}
//	return false;
//}
//
//
//static bool calc_full_house(const std::vector<card_t*>& cards, hand_board_result_t* result)
//{
//	hand_board_result_t trips_result; 
//	bool have_result = calc_trips(cards, &trips_result);
//	if (!have_result)
//	{
//		return false;
//	}
//	std::vector<card_t*> new_cards;
//	for (auto& card : cards)
//	{
//		if (card->rank != trips_result.kickers[0]->rank)
//		{
//			new_cards.emplace_back(card);
//		}
//	}
//	hand_board_result_t pair_result; 
//	have_result = calc_pair(new_cards, &pair_result);
//	if (!have_result)
//	{
//		return false;
//	}
//	result->strength = FULL_HOUSE;
//	result->kickers.emplace_back(trips_result.kickers[0]);
//	result->kickers.emplace_back(pair_result.kickers[0]);
//	return true;
//}
//
//
//static bool calc_two_pair(const std::vector<card_t*>& cards, hand_board_result_t* result)
//{
//	hand_board_result_t pair_result_1;
//	bool have_result = calc_pair(cards, &pair_result_1);
//	if (!have_result)
//	{
//		return false;
//	}
//	std::vector<card_t*> new_cards;
//	for (auto& card : cards)
//	{
//		if (card->rank != pair_result_1.kickers[0]->rank)
//		{
//			new_cards.emplace_back(card);
//		}
//	}
//	hand_board_result_t pair_result_2;
//	have_result = calc_pair(new_cards, &pair_result_2);
//	if (!have_result)
//	{
//		return false;
//	}
//	result->strength = TWO_PAIR;
//	card_t* kicker_1 = pair_result_1.kickers[0];
//	card_t* kicker_2 = pair_result_2.kickers[0];
//	if (kicker_1->rank < kicker_2->rank)
//	{
//		std::swap(kicker_1, kicker_2);
//	}
//	result->kickers.emplace_back(kicker_1);
//	result->kickers.emplace_back(kicker_2);
//	result->kickers.emplace_back(pair_result_2.kickers[1]);
//	return result;
//}
//
//
//static bool calc_high_card(const std::vector<card_t*>& cards, hand_board_result_t* result)
//{
//	result->strength = HIGH_CARD;
//	for (size_t i = 0; i < std::min((size_t)5, cards.size()); i++)
//	{
//		result->kickers.emplace_back(cards[i]);
//	}
//	return true;
//}
//
//
//static bool calc_straight(const std::vector<card_t*>& cards, hand_board_result_t* result)
//{
//	std::vector<card_t*> cards_normalized;
//	for (auto* i : cards)
//	{
//		if (cards_normalized.empty())
//		{
//			cards_normalized.push_back(i);
//			continue;
//		}
//		if (cards_normalized.back()->rank != i->rank)
//		{
//			cards_normalized.push_back(i);
//		}
//	}
//	card_t small_ace = { H, _A_1 };
//	if (cards_normalized[0]->rank == _A)
//	{
//		cards_normalized.push_back(&small_ace);
//	}
//	int consec = 1;
//	card_t* kicker;
//	bool have_result = false;
//	for (int i = (int)cards_normalized.size() - 2; i >= 0; i--)
//	{
//		if (cards_normalized[i]->rank == cards_normalized[i + 1]->rank + 1)
//		{
//			consec++;
//		}
//		else
//		{
//			consec = 1;
//		}
//		if (consec >= 5)
//		{
//			kicker = cards_normalized[i];
//			have_result = true;
//		}
//	}
//	if (have_result)
//	{
//		result->strength = STRAIGHT;
//		result->kickers.emplace_back(kicker);
//	}
//	return have_result;
//}
//
//
//static bool calc_flush(std::vector<card_t*> cards, hand_board_result_t* result)
//{
//	std::sort(all(cards), [](card_t* lhs, card_t* rhs)
//		{
//			if (lhs->color == rhs->color)
//			{
//				return lhs->rank > rhs->rank;
//			}
//			return lhs->color < rhs->color;
//		});
//	int consec = 1;
//	card_t* kicker;
//	bool have_result = false;
//	for (int i = (int)cards.size() - 2; i >= 0; i--)
//	{
//		if (cards[i]->color == cards[i + 1]->color)
//		{
//			consec++;
//		}
//		else
//		{
//			consec = 1;
//		}
//		if (consec >= 5)
//		{
//			kicker = cards[i];
//			have_result = true;
//		}
//	}
//	if (have_result)
//	{
//		result->strength = FLUSH;
//		result->kickers.emplace_back(kicker);
//	}
//	return have_result;
//}
