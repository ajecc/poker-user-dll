#pragma once

#include "card.h"
#include "board.h"


const int HAND_CARD_COUNT = 2;


struct hand_t
{
	card_t cards[HAND_CARD_COUNT];
};



double calc_prwin_vs_hand(hand_t* hero, hand_t* villain, board_t* board);
