#pragma once
#include "pch.h"
#include "../User/range.h"
#include "../User/util.h"


TEST(RangeTest, get_open_range)
{
	auto* range = get_open_range(CO);
	const range_hand_t* hand = range->fetch(get_hand(get_card(_T, H), get_card(_7, H)));
	ASSERT_EQ(hand->hand_action, FOLD);
	ASSERT_NEAR(hand->raise_prob, 0.04, EPS);
}


TEST(RangeTest, get_facing_raise_range)
{
	auto* range = get_facing_raise_range(BB, BTN);
	ASSERT_EQ(range->size(), 75);
	const range_hand_t* hand = nullptr;
	hand = range->fetch(get_hand(get_card(_2, H), get_card(_2, S)));
	ASSERT_EQ(hand->hand_action, CALL);
	ASSERT_NEAR(hand->raise_prob, 0.00, EPS);

	range = get_facing_raise_range(CO, HJ);
	ASSERT_EQ(range->size(), 24);
}


TEST(RangeTest, get_facing_3bet_ranges)
{
	auto* range = get_facing_3bet_range(CO, BB);
	ASSERT_EQ(range->size(), 53);
	const range_hand_t* hand = nullptr;
	hand = range->fetch(get_hand(get_card(_A, H), get_card(_J, S)));
	ASSERT_EQ(hand->hand_action, FOLD);
	ASSERT_NEAR(hand->raise_prob, 0.13, EPS);
}


TEST(RangeTest, get_facing_4bet_ranges)
{
	auto* range = get_facing_4bet_range(SB, UTG);
	ASSERT_EQ(range->size(), 4);
	const range_hand_t* hand = nullptr;
	hand = range->fetch(get_hand(get_card(_A, H), get_card(_K, H)));
	ASSERT_EQ(hand->hand_action, CALL);
	ASSERT_NEAR(hand->raise_prob, 0.73, EPS);
}
