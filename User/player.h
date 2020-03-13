#pragma once

#include <string>


constexpr int PLAYERS_COUNT = 6;

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
	double balance = 0;
	double current_bet = 0;
	double overall_agression = 0;
	double overall_agression_freq = 0;
	count_t hand_seen = 0;
	double overall_avg_hand_strength = 0;
	count_t win = 0;
	count_t lose = 0;
	count_t faceup_lose = 0;
	count_t faceup_win = 0;
	count_t bluff = 0;
	count_t position = 0;  // the number of players that speak after this player
	bool is_dealer = false;
	bool is_small = false;
	bool is_big = false;
	bool is_in_hand = false;
	bool is_in_game = false;  // used for folds/sitouts
	double range[PLAYERS_COUNT] = { 0 };  // the weakest cards this player plays in a position 

	board_stage_stats_t preflop = { 0 };
	board_stage_stats_t flop = { 0 };
	board_stage_stats_t turn = { 0 };
	board_stage_stats_t river = { 0 };

	std::string to_string();
};



player_t* get_current_player(board_t* board);

void update_player(player_t* player);



// TODO: make something that calculates the strength of a hand
