#pragma once

#include <vector>
#include "player.h"
#include "card.h"


struct board_derived_info_t;


struct board_t
{
	board_stage_t stage;
	std::vector<card_t*> cards;
	std::vector<player_t*> players;
	std::vector<player_t*> current_hand_players;
	double pot = 0;
	// TODO: get the big_blind_sum
	double big_blind_sum = 0;
	// TODO: update this
	board_derived_info_t* board_derived_info = nullptr;


	std::string to_string();
};


board_t* create_board();

void destroy_board(board_t** board);

player_t* get_player_by_label(board_t* board, const std::string& label);

void update_board(board_t* board);

player_t* get_next_player(board_t* board, player_t* player);

player_t* get_next_player_in_game(board_t* board, player_t* player);

player_t* get_next_player_in_hand(board_t* board, player_t* player);
