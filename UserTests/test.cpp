#include "pch.h"
#include "../User/user.h"
#include "../User/prwin_calc.h"
#include <iostream>
#include <future>

#include "test_card.h"
#include "test_hand_board_result.h"
#include "test_hand.h"
#include "test_range.h"
//#include "test_prwin_calc.h"



int 
main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	create_globals();
	auto result = RUN_ALL_TESTS();
	board_t board;
	board.cards =
	{ get_card("5c"), get_card("7d"), get_card("8d")};
	std::sort(all(board.cards), card_ptr_cmp);
	const hand_t* hero_hand = get_hand(get_card("Kc"), get_card("5s"));
	const hand_t* villain_hand = get_hand(get_card("Kd"), get_card("Ac"));
	float start = clock();
	float end = clock();
	std::cout << calc_prwin_vs_any_hand(hero_hand, &board) << '\n';
	float time = (end - start) / CLOCKS_PER_SEC;
	std::cout <<  "TIME: " << time << '\n';
	return result;
}
