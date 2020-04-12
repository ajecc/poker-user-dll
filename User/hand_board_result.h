#pragma once
#include "card.h"
#include "hand.h"
#include "board.h"
#include <vector>


enum hand_board_result_strength_t
{
	INVALID_STRENGTH = -1,
	HIGH_CARD = 0,
	PAIR = 1,
	TWO_PAIR = 2,
	TRIPS = 3,
	STRAIGHT = 4,
	FLUSH = 5, 
	FULL_HOUSE = 6,
	QUADS = 7,
	STRAIGHT_FLUSH = 8
};


struct hand_board_result_t
{
	rank_t kicker_4 : 5;
	rank_t kicker_3 : 5;
	rank_t kicker_2 : 5;
	rank_t kicker_1 : 5;
	rank_t kicker_0 : 5;
	hand_board_result_strength_t strength : 5;

	bool operator<(const hand_board_result_t& other) const;

	bool operator>(const hand_board_result_t& other) const;

	bool operator==(const hand_board_result_t& other) const;

	bool operator<=(const hand_board_result_t& other) const;

	bool operator>=(const hand_board_result_t& other) const;

	bool operator!=(const hand_board_result_t& other) const;
};


hand_board_result_t calc_hand_board_result_uncached(hand_t* hand, board_t* board);
