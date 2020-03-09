#pragma once

#include <string>


const int MAX_PLAYER_COUNT = 6;

typedef int count_t;

struct board_t;

enum board_stage_t 
{
	PREFLOP,
	FLOP,
	TURN,
	RIVER,
	BOARD_STAGE_COUNT
};


struct board_stage_stats_t
{
	count_t seen;
	double aggression;
	double aggression_freq;
	double avg_hand_strength; // the average strength of the hand that reached this stage
	count_t win;
	count_t lose;

	count_t fold;
	count_t check;
	count_t call;
	count_t bet;
	count_t raise;
	count_t folded_bet;
	count_t folded_raise;
	count_t called_raise;
	count_t reraise;
	count_t cbet;
	count_t fold_to_cbet;
	count_t called_cbet;
	count_t raised_cbet;

	// weighted in accordance pot size
	double weighted_avg_call;
	double weighted_avg_bet;
	double weighted_avg_raise;

	count_t dealer_raise;
	count_t small_raise;
};



struct player_t 
{
	std::string label;  // this label might be p6, for example, but the player might be Player 1
	std::string name;
	double balance;
	double current_bet;
	double overall_agression;
	double overall_agression_freq;
	count_t hand_seen;
	double overall_avg_hand_strength;
	count_t win;
	count_t lose;
	count_t faceup_lose;
	count_t faceup_win;
	count_t bluff;
	count_t position;  // the number of players that speak after this player
	bool is_dealer;
	bool is_small;
	bool is_big;
	bool is_in_hand;
	bool is_in_game;  // used for folds/sitouts
	double range[MAX_PLAYER_COUNT];  // the weakest cards this player plays in a position 

	board_stage_stats_t preflop;
	board_stage_stats_t flop;
	board_stage_stats_t turn;
	board_stage_stats_t river;
};



player_t* get_current_player(board_t* board);

void update_player(player_t* player);



// TODO: make something that calculates the strength of a hand
