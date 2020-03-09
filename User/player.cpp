#include "player.h"
#include "board.h"
#include "open_holdem_functions.h"
#include "poker_exception.h"


static void update_player_is_in_game(player_t* player);

static void update_player_balance(player_t* player);

static void update_player_current_bet(player_t* player);

static void update_player_is_in_hand(player_t* player);


void update_player(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	update_player_is_in_game(player);
	if (!player->is_in_game)
	{
		return;
	}
	update_player_balance(player);
	update_player_current_bet(player);
	update_player_is_in_hand(player);
}


player_t* get_current_player(board_t* board)
{
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
	std::string inactive_query_string = "pXinactive";
	for (int i = 0; i < MAX_PLAYER_COUNT; i++)
	{
		inactive_query_string[1] = (char)(i + (int)'0');
		std::string response = scrape_table_map_region(inactive_query_string);
		if (response[0] != 'p')
		{
			return get_player_by_label(
				board,
				std::string(inactive_query_string.begin(), inactive_query_string.begin() + 2)
			);
		}
	}
	throw poker_exception_t("Could not get current player");
}


static void update_player_is_in_game(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "seated";
	std::string query_response = scrape_table_map_region(query);
	if (query_response == "true")
	{
		player->is_in_game = true;
	}
	else
	{
		player->is_in_game = false;
	}
}


static void update_player_balance(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "query";
	player->balance = scrape_table_map_region_numeric(query);
}


static void update_player_current_bet(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "bet";
	player->current_bet = scrape_table_map_region_numeric(query);
	// TODO: check for raise, update player aggresivity etc. 
	// NOTE: maybe use poker tracer
}


static void update_player_is_in_hand(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "cardback";
	std::string query_response = scrape_table_map_region(query);
	if (query_response == "true")
	{
		player->is_in_hand = true;
	}
	else
	{
		player->is_in_hand = false;
	}
}