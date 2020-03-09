#include "board.h"
#include "poker_exception.h"
#include "util.h"
#include "open_holdem_functions.h"
#include <algorithm>

static void clear_board(board_t* board);

static void update_cards(board_t* board);

static void update_players(board_t* board);


board_t* create_board()
{
	auto* board = new board_t;
	if (nullptr == board)
	{
		goto cleanup;
	}
	for (int i = 0; i < PLAYERS_COUNT; i++)
	{
		auto* player = new player_t;
		if (player == nullptr)
		{
			goto cleanup;
		}
		board->players.emplace_back(player);
	}
	return board;
cleanup:
	destroy_board(&board);
}


void destroy_board(board_t** board)
{
	if (nullptr == board)
	{
		return;
	}
	board_t* board_deref = *board;
	if (nullptr != board_deref)
	{
		clear_board(board_deref);
		for (auto* player : board_deref->players)
		{
			delete player;
		}
		delete board_deref;
	}
	*board = nullptr;
}


player_t* get_player_by_label(board_t* board, const std::string& label)
{
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
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
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
	clear_board(board);
	update_cards(board);
}


static void clear_board(board_t* board)
{
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
	for (auto* card : board->cards)
	{
		delete card;
	}
	for (auto* current_hand_player : board->current_hand_players)
	{
		delete current_hand_player;
	}
	board->cards.clear();
	board->current_hand_players.clear();
}


static void update_cards(board_t* board)
{
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
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
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
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

std::string board_t::to_string()
{
	std::string to_string;
	to_string = "CARDS: ";
	for (auto* card : cards)
	{
		to_string += card->to_string() + " ";
	}
	to_string += "\n\n";
	to_string += "PLAYERS: \n";
	for (auto* player : players)
	{
		to_string += player->to_string();
		to_string += "\n";
	}
	return to_string;
}
