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
	{ get_card("Jc"), get_card("Qs"), get_card("As"), get_card("6c")};
	std::sort(all(board.cards), card_ptr_cmp);
	const hand_t* hero_hand = get_hand(get_card("9s"), get_card("Ah"));
	const hand_t* villain_hand = get_hand(get_card("Kd"), get_card("Ac"));
	float sum = 0;
	float start = clock();
	std::vector<std::future<float>> futures;
	std::vector<float> not_futures;
	for (int i = 0; i < 52 * 51; i++)
	{
		//futures.push_back(std::async(calc_prwin_vs_any_hand, hero_hand, &board));
		not_futures.push_back(calc_prwin_vs_any_hand(hero_hand, &board));
	}
	for (auto& future : not_futures)
	{
		//sum += future.get();
		sum += future;
	}
	float end = clock();
	std::cout << sum * 100 << "%" << '\n';
	float time = (end - start) / CLOCKS_PER_SEC;
	std::cout <<  "TIME: " << time << '\n';
	return result;
}
