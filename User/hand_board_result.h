#pragma once
#include "card.h"
#include "hand.h"
#include "board.h"
#include <vector>

#define ALL_HAND_BOARD_RESULT_CACHED_SHARED_NAME "Global\\ALL_HAND_BOARD_RESULT_CACHED_SHARED"
#define ALL_HAND_BOARD_RESULT_CACHED_FILE_NAME "all_hand_board_result.cache"


enum hand_board_result_strength_t
{
	INVALID_HAND_BOARD_RESULT_STRENGTH = -1,
	HIGH_CARD = 0,
	PAIR = 1,
	TWO_PAIR = 2,
	TRIPS = 3,
	STRAIGHT = 4,
	FLUSH = 5,
	FULL_HOUSE = 6,
	QUADS = 7,
	STRAIGHT_FLUSH = 8,
	HAND_BOARD_RESULT_STRENGTH_COUNT = 9
};


/*
This structures measures the strength of the hand and defines appropriate operators to
compare these strengths. The kickers are not placed in an array because of memory reasons =>
sizeof(hand_board_result_t) == 4, thus fitting in an UNSIGNED int and occupying little memory.
*/
struct hand_board_result_t
{
	// NOTE: don't modify this. There is a memory reason why the kickers are hardcoded like this.
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


/*
Calculates the hand_board_result for a particular hand, on a board that reached river
or simply put it has 5 cards down. This function doesn't use the cached results and is
thus very slow. Should not be used.

Parameters: IN hand_t* hand -- the hand to calculate the result for
			IN board_t* board -- the board with 5 cards down (only the cards vector is necesarry) 

Returns: hand_board_result_t
*/
hand_board_result_t calc_hand_board_result_uncached(const hand_t* hand, board_t* board);


/*
Calculates the hand_board_result for a particular hand, on a board that reached river
or simply put it has 5 cards down. This function uses the cached results and is fast.
The time to call this 169 is aprox 1 sec on a system with 4 threads.

Parameters: IN hand_t* hand -- the hand to calculate the result for
			IN board_t* board -- the board with 5 cards down (only the cards vector is necesarry) 

Returns: hand_board_result_t
*/
hand_board_result_t calc_hand_board_result(const hand_t* hand, board_t* board);


/*
Creates the cache to be used by calc_hand_board_result. This cache is created only once
and shared among processes. This should be called exactly once per process, when the DLL
is attaching.

Parameters: none

Returns: hand_board_result_t* -- a pointer to a memory region that contains the cache.
								This memory region should not be dirrectly accessed.
								Use calc_hand_board_result for this.
*/
const hand_board_result_t* create_all_hand_board_results();
