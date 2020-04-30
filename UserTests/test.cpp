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
	auto result =  RUN_ALL_TESTS();
	board_t board;
	board.cards =
	{ get_card("8d"), get_card("Ac"), get_card("7h"), get_card("7d"), get_card("2h")};
	const hand_t* hero_hand = get_hand(get_card("3d"), get_card("7s"));
	const hand_t* villain_hand = get_hand(get_card("Kd"), get_card("Ac"));
	float start = clock();
	float sum = 0;
	sum += calc_prwin_vs_any_hand(hero_hand, &board);
	std::cout << sum * 100 << "%" << '\n';
	std::cout << calc_prwin_vs_hand(hero_hand, villain_hand, &board) * 100 << "%\n";
	float end = clock();
	float time = (end - start) / CLOCKS_PER_SEC;
	std::cout <<  "TIME: " << time << '\n';
	return result;
}
