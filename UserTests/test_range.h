#pragma once
#include "pch.h"
#include "../User/hero_preflop_range.h"
#include "../User/util.h"


TEST(RangeTest, get_open_hero_preflop_range)
{
	auto* hero_preflop_range = get_hero_preflop_range(CO, INVALID_POSITION, OPEN);
	const hero_preflop_range_hand_t* hand = hero_preflop_range->fetch(get_hand(get_card(_T, H), get_card(_7, H)));
	ASSERT_EQ(hand->hand_action, FOLD);
	ASSERT_NEAR(hand->raise_prob, 0.04, EPS);
}


TEST(RangeTest, get_facing_raise_hero_preflop_range)
{
	auto* hero_preflop_range = get_hero_preflop_range(BB, BTN, FACING_RAISE);
	ASSERT_EQ(hero_preflop_range->size(), 75);
	const hero_preflop_range_hand_t* hand = nullptr;
	hand = hero_preflop_range->fetch(get_hand(get_card(_2, H), get_card(_2, S)));
	ASSERT_EQ(hand->hand_action, CALL);
	ASSERT_NEAR(hand->raise_prob, 0.00, EPS);

	hero_preflop_range = get_hero_preflop_range(CO, HJ, FACING_RAISE);
	ASSERT_EQ(hero_preflop_range->size(), 24);
}


TEST(RangeTest, get_facing_3bet_ranges)
{
	auto* hero_preflop_range = get_hero_preflop_range(CO, BB, FACING_3BET);
	ASSERT_EQ(hero_preflop_range->size(), 53);
	const hero_preflop_range_hand_t* hand = nullptr;
	hand = hero_preflop_range->fetch(get_hand(get_card(_A, H), get_card(_J, S)));
	ASSERT_EQ(hand->hand_action, FOLD);
	ASSERT_NEAR(hand->raise_prob, 0.13, EPS);
}


TEST(RangeTest, get_facing_4bet_ranges)
{
	auto* hero_preflop_range = get_hero_preflop_range(SB, UTG, FACING_4BET);
	ASSERT_EQ(hero_preflop_range->size(), 4);
	const hero_preflop_range_hand_t* hand = nullptr;
	hand = hero_preflop_range->fetch(get_hand(get_card(_A, H), get_card(_K, H)));
	ASSERT_EQ(hand->hand_action, CALL);
	ASSERT_NEAR(hand->raise_prob, 0.73, EPS);
}
