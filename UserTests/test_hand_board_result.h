#pragma once
#include "pch.h"
#include "../User/hand_board_result.h"


TEST(HandBoardResult, calc_hand_board_result_straight_flush)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_J, C),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_3, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_A, H), get_card(_5, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _5);

	board.cards = {
		get_card(_7, H),
		get_card(_J, C),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, H), get_card(_3, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _7);

	board.cards = {
		get_card(_7, H),
		get_card(_3, H),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, H), get_card(_3, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _7);

	board.cards = {
		get_card(_7, H),
		get_card(_8, H),
		get_card(_4, H),
		get_card(_6, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, S), get_card(_6, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _8);

	board.cards = {
		get_card(_2, H),
		get_card(_3, H),
		get_card(_T, H),
		get_card(_J, H),
		get_card(_Q, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_A, H), get_card(_K, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _A);
}


TEST(HandBoardResult, calc_hand_board_result_straight)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_J, C),
		get_card(_4, D),
		get_card(_K, S),
		get_card(_3, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_A, H), get_card(_5, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _5);

	board.cards = {
		get_card(_7, H),
		get_card(_J, C),
		get_card(_4, S),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, H), get_card(_3, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _7);

	board.cards = {
		get_card(_7, H),
		get_card(_3, S),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, H), get_card(_3, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _7);

	board.cards = {
		get_card(_7, C),
		get_card(_8, S),
		get_card(_4, D),
		get_card(_6, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, S), get_card(_6, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _8);

	board.cards = {
		get_card(_2, S),
		get_card(_3, C),
		get_card(_T, D),
		get_card(_J, H),
		get_card(_Q, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_A, H), get_card(_K, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _A);
}


TEST(HandBoardResult, calc_hand_board_result_flush)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_J, C),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_3, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_T, H), get_card(_5, H)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _T);

	board.cards = {
		get_card(_7, H),
		get_card(_J, C),
		get_card(_4, S),
		get_card(_K, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, H), get_card(_3, H)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _K);

	board.cards = {
		get_card(_7, H),
		get_card(_2, H),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, H), get_card(_3, C)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _7);

	board.cards = {
		get_card(_2, H),
		get_card(_8, H),
		get_card(_4, H),
		get_card(_6, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_6, S), get_card(_6, C)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _8);

	board.cards = {
		get_card(_2, H),
		get_card(_3, H),
		get_card(_9, H),
		get_card(_J, H),
		get_card(_Q, H)
			};
	result = calc_hand_board_result(
		get_hand(get_card(_A, H), get_card(_K, H)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kickers.size(), 1);
	ASSERT_EQ(result.kickers[0]->rank, _A);
}


TEST(HandBoardResult, calc_hand_board_result_quads)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_T, C),
		get_card(_4, H),
		get_card(_T, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_T, H), get_card(_T, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _T);
	ASSERT_EQ(result.kickers[1]->rank, _4);

	board.cards = {
		get_card(_2, H),
		get_card(_T, C),
		get_card(_4, H),
		get_card(_T, D),
		get_card(_T, S)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_T, H), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _T);
	ASSERT_EQ(result.kickers[1]->rank, _A);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_T, S),
		get_card(_T, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, H), get_card(_K, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _T);
	ASSERT_EQ(result.kickers[1]->rank, _A);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_T, S),
		get_card(_T, D),
		get_card(_Q, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, H), get_card(_K, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _T);
	ASSERT_EQ(result.kickers[1]->rank, _K);
}


TEST(HandBoardResult, calc_hand_board_result_full_house)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_K, H),
		get_card(_T, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_T, H), get_card(_T, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _T);
	ASSERT_EQ(result.kickers[1]->rank, _K);

	board.cards = {
		get_card(_2, H),
		get_card(_T, C),
		get_card(_4, H),
		get_card(_T, D),
		get_card(_2, S)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_T, H), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _T);
	ASSERT_EQ(result.kickers[1]->rank, _2);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_K, S),
		get_card(_K, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, H), get_card(_T, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _K);
	ASSERT_EQ(result.kickers[1]->rank, _T);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_K, S),
		get_card(_K, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_A, C), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _A);
	ASSERT_EQ(result.kickers[1]->rank, _K);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_K, S),
		get_card(_K, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, C), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kickers.size(), 2);
	ASSERT_EQ(result.kickers[0]->rank, _K);
	ASSERT_EQ(result.kickers[1]->rank, _A);
}


TEST(HandBoardResult, calc_hand_board_result_trips)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_K, H),
		get_card(_T, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, TRIPS);
	ASSERT_EQ(result.kickers.size(), 3);
	ASSERT_EQ(result.kickers[0]->rank, _K);
	ASSERT_EQ(result.kickers[1]->rank, _Q);
	ASSERT_EQ(result.kickers[2]->rank, _T);

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_K, H),
		get_card(_T, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, S), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, TRIPS);
	ASSERT_EQ(result.kickers.size(), 3);
	ASSERT_EQ(result.kickers[0]->rank, _K);
	ASSERT_EQ(result.kickers[1]->rank, _A);
	ASSERT_EQ(result.kickers[2]->rank, _T);
}


TEST(HandBoardResult, calc_hand_board_result_two_pair)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_T, H),
		get_card(_T, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, TWO_PAIR);
	ASSERT_EQ(result.kickers.size(), 3);
	ASSERT_EQ(result.kickers[0]->rank, _K);
	ASSERT_EQ(result.kickers[1]->rank, _T);
	ASSERT_EQ(result.kickers[2]->rank, _Q);

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_K, H),
		get_card(_3, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_2, S), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, TWO_PAIR);
	ASSERT_EQ(result.kickers.size(), 3);
	ASSERT_EQ(result.kickers[0]->rank, _K);
	ASSERT_EQ(result.kickers[1]->rank, _3);
	ASSERT_EQ(result.kickers[2]->rank, _A);
}


TEST(HandBoardResult, calc_hand_board_result_pair)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_T, H),
		get_card(_9, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, PAIR);
	ASSERT_EQ(result.kickers.size(), 4);
	ASSERT_EQ(result.kickers[0]->rank, _K);
	ASSERT_EQ(result.kickers[1]->rank, _Q);
	ASSERT_EQ(result.kickers[2]->rank, _T);
	ASSERT_EQ(result.kickers[3]->rank, _9);
}


TEST(HandBoardResult, calc_hand_board_result_high_card)
{
	board_t board;
	hand_board_result_t result;

	board.cards = {
		get_card(_2, H),
		get_card(_A, C),
		get_card(_T, H),
		get_card(_9, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, HIGH_CARD);
	ASSERT_EQ(result.kickers.size(), 5);
	ASSERT_EQ(result.kickers[0]->rank, _A);
	ASSERT_EQ(result.kickers[1]->rank, _K);
	ASSERT_EQ(result.kickers[2]->rank, _Q);
	ASSERT_EQ(result.kickers[3]->rank, _T);
	ASSERT_EQ(result.kickers[4]->rank, _9);
}

