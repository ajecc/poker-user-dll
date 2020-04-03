#include "pch.h"
#include "../User/user.h"
#include <iostream>

#include "test_card.h"
#include "test_hand_board_result.h"
#include "test_hand.h"
#include "test_range.h"



int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	create_globals();
	board_t board;
	board.cards = { get_card(_7, S), get_card(_J, D), get_card(_2, C) };
	hand_t* hero_hand = get_hand(get_card(_A, H), get_card(_A, C));
	hand_t* villain_hand = get_hand(get_card(_T, H), get_card(_6, C));
	std::cout << calc_prwin_vs_hand(hero_hand, villain_hand, &board) << '\n';
	double start = clock();
	std::cout << calc_prwin_vs_any_hand(hero_hand, &board) << '\n';
	double end = clock();
	double time = (end - start) / CLOCKS_PER_SEC;
	std::cout << time << '\n';
	return RUN_ALL_TESTS();
}
