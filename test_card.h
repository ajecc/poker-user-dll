#pragma once
#include "pch.h"
#include "../User/card.h"

TEST(CardTest, CardTest)
{
	card_t* card = get_card(_A, H);
	EXPECT_EQ(card->color, H);
}
