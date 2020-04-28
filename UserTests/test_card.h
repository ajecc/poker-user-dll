#pragma once
#include "pch.h"
#include "../User/card.h"


TEST(CardTest, get_card)
{
	const card_t* card = nullptr;

	card = get_card(_A, H);
	EXPECT_EQ(card->rank, _A);
	EXPECT_EQ(card->color, H);

	card = get_card(_6, C);
	EXPECT_EQ(card->rank, _6);
	EXPECT_EQ(card->color, C);

	card = get_card(_T, D);
	EXPECT_EQ(card->rank, _T);
	EXPECT_EQ(card->color, D);

	card = get_card(_2, H);
	EXPECT_EQ(card->rank, _2);
	EXPECT_EQ(card->color, H);

	card = get_card(_2, S);
	EXPECT_EQ(card->rank, _2);
	EXPECT_EQ(card->color, S);

	card = get_card(_A, S);
	EXPECT_EQ(card->rank, _A);
	EXPECT_EQ(card->color, S);
}


TEST(CardTest, get_card_from_string)
{
	const card_t* card = nullptr;

	card = get_card("As");
	EXPECT_EQ(card->rank, _A);
	EXPECT_EQ(card->color, S);

	card = get_card("2h");
	EXPECT_EQ(card->rank, _2);
	EXPECT_EQ(card->color, H);

	card = get_card("6c");
	EXPECT_EQ(card->rank, _6);
	EXPECT_EQ(card->color, C);
}
