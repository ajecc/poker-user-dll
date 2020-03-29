#include "board.h"
#include "poker_exception.h"
#include "util.h"
#include "open_holdem_functions.h"
#include "debug.h"
#include <algorithm>

static void clear_board(board_t* board);

static void update_cards(board_t* board);

static void update_players(board_t* board);

static void update_player_positions(board_t* board);


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
		player->label = "p" + std::to_string(i);
		board->players.emplace_back(player);
	}
	return board;
cleanup:
	destroy_board(&board);
	return nullptr;
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

	std::string error_message = "Player with label ";
	error_message += label + " not found";
	throw poker_exception_t(error_message);
}


void update_board(board_t* board)
{
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
	clear_board(board);
	update_cards(board);
	update_players(board);
	update_player_positions(board);
	DLOG(INFO, board->to_string().c_str());
}


player_t* get_next_player(board_t* board, player_t* player)
{
	std::string label = player->label;
	label[1] = (label[1] - '0' + 1) % PLAYERS_COUNT + '0';
	return get_player_by_label(board, label);
}


player_t* get_next_player_in_game(board_t* board, player_t* player)
{
	for (int i = 0; i < PLAYERS_COUNT + 1; i++)
	{
		player = get_next_player(board, player);
		if (player->is_in_game)
		{
			return player;
		}
	}
	throw poker_exception_t("Could not get next player in game");
}


player_t* get_next_player_in_hand(board_t* board, player_t* player)
{
	for (int i = 0; i < PLAYERS_COUNT + 1; i++)
	{
		player = get_next_player(board, player);
		if (player->is_in_hand)
		{
			return player;
		}
	}
	throw poker_exception_t("Could not get next player in hand");
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
	for (int i = 0; i < PLAYERS_COUNT; i++)
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
	for (char i = '0'; i < '0' + PLAYERS_COUNT; i++)
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


static void update_player_positions(board_t* board)
{
	DLOG(INFO, "in update_player_positions");
	for (auto* player : board->players)
	{
		player->is_dealer = false;
		player->is_small = false;
		player->is_big = false;
	}
	std::string dealer_label = "pX";
	player_t* big_blind = nullptr;
	player_t* dealer = nullptr;
	player_t* small_blind = nullptr;
	for (char i = '0'; i < '0' + PLAYERS_COUNT; i++)
	{
		dealer_label[1] = i;
		std::string query = dealer_label + "dealer";
		std::string query_response = scrape_table_map_region(query);
		if (query_response == "true")
		{
			dealer = get_player_by_label(board, dealer_label);
			dealer->is_dealer = true;
			// heads up case:
			if (board->players.size() == 2)
			{
				dealer->is_small = true;
				small_blind = dealer;
			}
			else
			{
				small_blind = get_next_player_in_game(board, dealer);
				small_blind->is_small = true;
			}
			big_blind = get_next_player_in_game(board, small_blind);
			big_blind->is_big = true;
			break;
		}
	}
	if (dealer == nullptr || small_blind == nullptr || big_blind == nullptr)
	{
		throw poker_exception_t("Could not find dealer/small/big");
	}
	player_t* current_player;
	if (board->stage == PREFLOP)
	{
		DLOG(INFO, "preflop");
		current_player = get_next_player_in_hand(board, big_blind);
	}
	else
	{
		DLOG(INFO, "not preflop");
		current_player = get_next_player_in_hand(board, dealer);
	}

	int in_front_cnt = 0;
	player_t* in_front = get_next_player_in_hand(board, current_player);
	while(in_front != current_player)
	{
		in_front_cnt++;
		in_front = get_next_player_in_hand(board, in_front);
	}
	current_player->position = in_front_cnt;

	player_t* stopping_point = current_player;
	for(;;)
	{
		DLOG(INFO, "pos label = %s", current_player->label.c_str());
		player_t* next_player = get_next_player_in_hand(board, current_player);
		if (next_player == stopping_point)
		{
			break;
		}
		next_player->position = current_player->position - 1;
		current_player = next_player;
	} 
	DLOG(INFO, "exit update_player_positions");
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