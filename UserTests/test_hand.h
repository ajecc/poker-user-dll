#pragma once
#include "pch.h"
#include "../User/hand.h"


TEST(HandTest, get_hand)
{
	const hand_t* hand = nullptr;
	const card_t* card_lhs = nullptr;
	const card_t* card_rhs = nullptr;

	std::vector<std::tuple<rank_t, color_t, rank_t, color_t>> tests;
	tests.push_back({ _A, C, _A, D });
	tests.push_back({ _2, D, _3, D });
	tests.push_back({ _2, H, _2, S });
	tests.push_back({ _J, S, _T, C });
	tests.push_back({ _Q, C, _9, D });

	for (auto& test : tests)
	{
		card_lhs = get_card(std::get<0>(test), std::get<1>(test));
		card_rhs = get_card(std::get<2>(test), std::get<3>(test));
		if (*card_lhs > *card_rhs)
		{
			std::swap(card_lhs, card_rhs);
		}
		hand = get_hand(card_lhs, card_rhs);
		ASSERT_EQ(hand->cards[0], card_lhs);
		ASSERT_EQ(hand->cards[1], card_rhs);
		ASSERT_EQ(hand->suited, card_lhs->color == card_rhs->color);
	}
}


TEST(HandTest, is_hero_winner)
{
	board_t* board = new board_t;
	const hand_t* hero = nullptr;
	const hand_t* villain = nullptr;
	std::vector < std::tuple<const hand_t*, const hand_t*, std::vector<const card_t*>>> tests;

	tests.push_back({
		get_hand(get_card(_A, H), get_card(_A, S)),
		get_hand(get_card(_K, H), get_card(_K, S)),
		{get_card(_2, H), get_card(_3, H), get_card(_4, S), get_card(_J, S), get_card(_7, C)},
		});

	tests.push_back({
		get_hand(get_card(_2, C), get_card(_2, S)),
		get_hand(get_card(_Q, H), get_card(_Q, S)),
		{get_card(_2, H), get_card(_3, H), get_card(_K, S), get_card(_J, S), get_card(_7, C)},
		});

	tests.push_back({
		get_hand(get_card(_A, H), get_card(_Q, H)),
		get_hand(get_card(_K, H), get_card(_J, C)),
		{get_card(_2, H), get_card(_3, H), get_card(_4, H), get_card(_J, S), get_card(_7, C)},
		});

	tests.push_back({
		get_hand(get_card(_5, H), get_card(_6, H)),
		get_hand(get_card(_A, H), get_card(_K, H)),
		{get_card(_2, H), get_card(_3, H), get_card(_4, S), get_card(_J, S), get_card(_7, C)},
		});

	tests.push_back({
		get_hand(get_card(_4, H), get_card(_J, C)),
		get_hand(get_card(_A, H), get_card(_K, H)),
		{get_card(_2, H), get_card(_3, H), get_card(_4, S), get_card(_J, S), get_card(_4, C)},
		});


	for (auto& test : tests)
	{
		hero = std::get<0>(test);
		villain = std::get<1>(test);
		board->cards = std::get<2>(test);
		ASSERT_TRUE(is_hero_winner(hero, villain, board));
		ASSERT_FALSE(is_hero_winner(villain, hero, board));
	}
}
