#include "board.h"
#include "poker_exception.h"
#include "util.h"
#include "open_holdem_functions.h"
#include <algorithm>

static void clean_board(board_t* board);

static void update_cards(board_t* board);

static void update_players(board_t* board);


player_t* get_player_by_label(board_t* board, const std::string& label)
{
	for (auto* player : board->players)
	{
		if (label == player->label)
		{
			return player;
		}
	}
	throw poker_exception_t("Player by label not found");
}


void update_board(board_t* board)
{
	clean_board(board);
	update_cards(board);
}


static void clean_board(board_t* board)
{
	for (auto* card : board->cards)
	{
		delete card;
	}
	for (auto* player : board->players)
	{
		delete player;
	}
	for (auto* current_hand_player : board->current_hand_players)
	{
		delete current_hand_player;
	}
	board->cards.clear();
	board->players.clear();
	board->current_hand_players.clear();
}


static void update_cards(board_t* board)
{
	std::string cards_query_string = "c0cardfaceX";
	for (int i = 0; i < 5; i++)
	{
		cards_query_string[cards_query_string.size() - 1] = (char)(i + '0');
		std::string card_str = scrape_table_map_region(cards_query_string);
		card_t* card = get_card_from_string(card_str);
		if (card == nullptr)
		{
			goto cleanup;
		}
		board->cards.emplace_back(card);
	}
cleanup:
	switch (board->cards.size())
	{
	case 0:
		board->stage = PREFLOP;
		break;
	case 3:
		board->stage = FLOP;
		break;
	case 4:
		board->stage = TURN;
		break;
	case 5:
		board->stage = RIVER;
		break;
	default:
		throw new poker_exception_t("Can't determine board stage...");
	}
}


static void update_players(board_t* board)
{
	std::string player_label = "pX";
	for (char i = '0'; i < '6'; i++)
	{
		player_label[1] = i;
		player_t* player = get_player_by_label(board, player_label);
		update_player(player);
	}
	for (auto* player : board->players)
	{
		if (player->is_in_hand)
		{
			board->current_hand_players.emplace_back(player);
		}
	}
}
