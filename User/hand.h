#pragma once

#include "card.h"
#include "board.h"
#include <vector>


const int HAND_CARD_COUNT = 2;

enum hand_action_t
{
	FOLD = 0,
	CHECK = 1,
	CALL = 2,
	RAISE = 3
};

hand_action_t get_hand_action_from_char(char hand_action_char);


struct hand_t
{
	card_t* cards[HAND_CARD_COUNT];
	bool suited;

	bool operator<(const hand_t& other) const;
};

std::vector<hand_t*> create_all_hands();

hand_t* get_hand(card_t* card_1, card_t* card_2);

bool is_hero_winner(hand_t* hero, hand_t* villain, board_t* board);

double calc_prwin_vs_hand(hand_t* hero, hand_t* villain, board_t* board);

double calc_prwin_vs_any_hand(hand_t* hero, board_t* board);

bool are_similar_hands(hand_t* lhs, hand_t* rhs);
