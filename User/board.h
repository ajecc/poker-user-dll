#pragma once

#include <vector>
#include "player.h"
#include "card.h"



struct board_t
{
	board_stage_t stage;
	std::vector<card_t*> cards;
	std::vector<player_t*> players;
	std::vector<player_t*> current_hand_players;
	double_t pot;
};


player_t* get_player_by_label(board_t* board, const std::string& label);

void update_board(board_t* board);
