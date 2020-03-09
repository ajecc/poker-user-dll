#pragma once

#include "card.h"
#include "board.h"


const int HAND_CARD_COUNT = 2;


struct hand_t
{
	card_t cards[HAND_CARD_COUNT];
	double strength;
};


double calculate_raw_hand_strength(hand_t* hand);
