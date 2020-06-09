#pragma once
#include "pch.h"
#include "../User/user.h"
#include "../User/prwin_calc.h"

extern std::vector<float> g_all_prwin_vs_any_hand_flop;
extern std::vector<const card_t*> g_all_cards;

TEST(PrwinCalc, calc_prwin_vs_any_hand_flop)
{
	size_t cur_ind = 0;
	size_t i = 0, j = 0, k = 0, l = 0, m = 0;
	for (i; i < 52; i++)
	{
		if (j <= i || j == 52)
		{
			j = i + 1;
		}
		for (j; j < 52; j++)
		{
			if (k == 52)
			{
				k = 0;
			}
			for (k; k < 52; k++)
			{
				if (l <= k || l == 52)
				{
					l = k + 1;
				}
				for (l; l < 52; l++)
				{
					if (m <= l || m == 52)
					{
						m = l + 1;
					}
					for (m; m < 52; m++)
					{
						if (k == j || l == j || m == j ||
							k == i || l == i || m == i)
						{
							continue;
						}
						const hand_t* hero_hand = 
							get_hand(g_all_cards[i], g_all_cards[j]);
						board_t board;
						board.cards = { g_all_cards[k], g_all_cards[l], g_all_cards[m] };
						EXPECT_FLOAT_EQ(calc_prwin_vs_any_hand(hero_hand, &board), g_all_prwin_vs_any_hand_flop[cur_ind]);
						cur_ind++;
					}
				}
			}
		}
	}
}
