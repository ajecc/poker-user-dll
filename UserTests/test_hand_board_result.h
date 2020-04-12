#pragma once
#include "pch.h"
#include "../User/hand_board_result.h"


TEST(HandBoardResult, calc_hand_board_result_uncached_straight_flush)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_A, H), get_card(_5, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kicker_0, _5);

	board.cards = {
		get_card(_7, H),
		get_card(_J, C),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, H), get_card(_3, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kicker_0, _7);

	board.cards = {
		get_card(_7, H),
		get_card(_3, H),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, H), get_card(_3, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kicker_0, _7);

	board.cards = {
		get_card(_7, H),
		get_card(_8, H),
		get_card(_4, H),
		get_card(_6, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, S), get_card(_6, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kicker_0, _8);

	board.cards = {
		get_card(_2, H),
		get_card(_3, H),
		get_card(_T, H),
		get_card(_J, H),
		get_card(_Q, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_A, H), get_card(_K, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT_FLUSH);
	ASSERT_EQ(result.kicker_0, _A);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_straight)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_A, H), get_card(_5, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kicker_0, _5);

	board.cards = {
		get_card(_7, H),
		get_card(_J, C),
		get_card(_4, S),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, H), get_card(_3, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kicker_0, _7);

	board.cards = {
		get_card(_7, H),
		get_card(_3, S),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, H), get_card(_3, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kicker_0, _7);

	board.cards = {
		get_card(_7, C),
		get_card(_8, S),
		get_card(_4, D),
		get_card(_6, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, S), get_card(_6, C)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kicker_0, _8);

	board.cards = {
		get_card(_2, S),
		get_card(_3, C),
		get_card(_T, D),
		get_card(_J, H),
		get_card(_Q, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_A, H), get_card(_K, H)),
		&board
	);
	ASSERT_EQ(result.strength, STRAIGHT);
	ASSERT_EQ(result.kicker_0, _A);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_flush)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_T, H), get_card(_5, H)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kicker_0, _T);

	board.cards = {
		get_card(_7, H),
		get_card(_J, C),
		get_card(_4, S),
		get_card(_K, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, H), get_card(_3, H)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kicker_0, _K);

	board.cards = {
		get_card(_7, H),
		get_card(_2, H),
		get_card(_4, H),
		get_card(_K, S),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, H), get_card(_3, C)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kicker_0, _7);

	board.cards = {
		get_card(_2, H),
		get_card(_8, H),
		get_card(_4, H),
		get_card(_6, H),
		get_card(_5, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_6, S), get_card(_6, C)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kicker_0, _8);

	board.cards = {
		get_card(_2, H),
		get_card(_3, H),
		get_card(_9, H),
		get_card(_J, H),
		get_card(_Q, H)
			};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_A, H), get_card(_K, H)),
		&board
	);
	ASSERT_EQ(result.strength, FLUSH);
	ASSERT_EQ(result.kicker_0, _A);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_quads)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_T, H), get_card(_T, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kicker_0, _T);
	ASSERT_EQ(result.kicker_1, _4);

	board.cards = {
		get_card(_2, H),
		get_card(_T, C),
		get_card(_4, H),
		get_card(_T, D),
		get_card(_T, S)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_T, H), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kicker_0, _T);
	ASSERT_EQ(result.kicker_1, _A);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_T, S),
		get_card(_T, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, H), get_card(_K, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kicker_0, _T);
	ASSERT_EQ(result.kicker_1, _A);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_T, S),
		get_card(_T, D),
		get_card(_Q, H)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, H), get_card(_K, S)),
		&board
	);
	ASSERT_EQ(result.strength, QUADS);
	ASSERT_EQ(result.kicker_0, _T);
	ASSERT_EQ(result.kicker_1, _K);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_full_house)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_T, H), get_card(_T, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kicker_0, _T);
	ASSERT_EQ(result.kicker_1, _K);

	board.cards = {
		get_card(_2, H),
		get_card(_T, C),
		get_card(_4, H),
		get_card(_T, D),
		get_card(_2, S)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_T, H), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kicker_0, _T);
	ASSERT_EQ(result.kicker_1, _2);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_K, S),
		get_card(_K, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, H), get_card(_T, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kicker_0, _K);
	ASSERT_EQ(result.kicker_1, _T);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_K, S),
		get_card(_K, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_A, C), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kicker_0, _A);
	ASSERT_EQ(result.kicker_1, _K);

	board.cards = {
		get_card(_T, H),
		get_card(_T, C),
		get_card(_K, S),
		get_card(_K, D),
		get_card(_A, H)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, C), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, FULL_HOUSE);
	ASSERT_EQ(result.kicker_0, _K);
	ASSERT_EQ(result.kicker_1, _A);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_trips)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, TRIPS);
	ASSERT_EQ(result.kicker_0, _K);
	ASSERT_EQ(result.kicker_1, _Q);
	ASSERT_EQ(result.kicker_2, _T);

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_K, H),
		get_card(_T, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, S), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, TRIPS);
	ASSERT_EQ(result.kicker_0, _K);
	ASSERT_EQ(result.kicker_1, _A);
	ASSERT_EQ(result.kicker_2, _T);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_two_pair)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, TWO_PAIR);
	ASSERT_EQ(result.kicker_0, _K);
	ASSERT_EQ(result.kicker_1, _T);
	ASSERT_EQ(result.kicker_2, _Q);

	board.cards = {
		get_card(_2, H),
		get_card(_K, C),
		get_card(_K, H),
		get_card(_3, D),
		get_card(_3, H)
	};
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_2, S), get_card(_A, S)),
		&board
	);
	ASSERT_EQ(result.strength, TWO_PAIR);
	ASSERT_EQ(result.kicker_0, _K);
	ASSERT_EQ(result.kicker_1, _3);
	ASSERT_EQ(result.kicker_2, _A);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_pair)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, PAIR);
	ASSERT_EQ(result.kicker_0, _K);
	ASSERT_EQ(result.kicker_1, _Q);
	ASSERT_EQ(result.kicker_2, _T);
	ASSERT_EQ(result.kicker_3, _9);
}


TEST(HandBoardResult, calc_hand_board_result_uncached_high_card)
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
	result = calc_hand_board_result_uncached(
		get_hand(get_card(_K, S), get_card(_Q, S)),
		&board
	);
	ASSERT_EQ(result.strength, HIGH_CARD);
	ASSERT_EQ(result.kicker_0, _A);
	ASSERT_EQ(result.kicker_1, _K);
	ASSERT_EQ(result.kicker_2, _Q);
	ASSERT_EQ(result.kicker_3, _T);
	ASSERT_EQ(result.kicker_4, _9);
}

