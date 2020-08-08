#include "player.h"
#include "board.h"
#include "open_holdem_functions.h"
#include "poker_exception.h"
#include "hand.h"
#include "villain_range.h"


static void
update_player_is_in_game(player_t* player);

static void
update_player_balance(player_t* player);

static void
update_player_current_bet(player_t* player);

static void
update_player_name(player_t* player);

static void
update_player_is_in_hand(player_t* player);

static void
update_player_cards(player_t* player);



void 
update_player(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	update_player_is_in_game(player);
	update_player_is_in_hand(player);
	if (!player->is_in_hand)
	{
		return;
	}
	update_player_balance(player);
	update_player_current_bet(player);
	update_player_name(player);
	if (player->is_hero)
	{
		update_player_cards(player);
	}
}


static void
update_player_is_in_game(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "seated";
	std::string query_response = scrape_table_map_region(query);
	if (query_response == "true" || query_response == "1")
	{
		player->is_in_game = true;
	}
	else
	{
		player->is_in_game = false;
	}
}


static void
update_player_balance(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "balance";
	player->balance = scrape_table_map_region_numeric(query);
}


static void
update_player_current_bet(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "bet";
	player->current_bet = scrape_table_map_region_numeric(query);
}


static void 
update_player_name(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	std::string query = player->label + "name";
	std::string query_response = scrape_table_map_region(query);
	player->name = query_response;
}


static void
update_player_is_in_hand(player_t* player)
{
	if (nullptr == player)
	{
		throw poker_exception_t("Player should never be nullptr");
	}
	player->is_in_hand = false;
	std::string query = player->label + "active";
	std::string query_response = scrape_table_map_region(query);
	// NOTE: p2 = hero's label
	if (query_response == "true" || query_response == "1" ||
		player->label == "p2")
	{
		player->is_in_hand = true;
	}
}


static void
update_player_cards(player_t* player)
{
	std::string query = player->label + "cardface0";
	std::string query_response = scrape_table_map_region(query);
	const card_t* card_0 = get_card(query_response);
	query = player->label + "cardface1";
	query_response = scrape_table_map_region(query);
	const card_t* card_1 = get_card(query_response);
	if (*card_0 > *card_1)
	{
		std::swap(card_0, card_1);
	}
	player->hand = get_hand(card_0, card_1);
}


std::string 
player_t::to_string() const
{
	std::string to_string = "label = ";
	to_string += label + "\n";
	if (!is_in_game)
	{
		to_string += "not in game";
		return to_string;
	}
	to_string += "name = " + name + "\n";
	to_string += "balance = " + std::to_string(balance) + "\n";
	to_string += "in_front = " + std::to_string(in_front) + "\n";
	to_string += "position = ";
	switch (position)
	{
	case BB:
		to_string += "BB (5)";
		break;
	case SB:
		to_string += "SB (4)";
		break;
	case BTN:
		to_string += "BTN (3)";
		break;
	case CO:
		to_string += "CO (2)";
		break;
	case HJ:
		to_string += "HJ (1)";
		break;
	case UTG:
		to_string += "UTG (0)";
		break;
	default:
		to_string += "INVALID";
	}
	to_string += "\n";
	to_string += "is_in_hand = ";
	if (is_in_hand)
	{
		to_string += "true\n";
	}
	else
	{
		to_string += "false\n";
	}
	if (is_in_hand && !is_hero)
	{
		std::string villain_range_string = "[...]";
		if (villain_range->villain_range.size() <= 16)
		{
			villain_range_string = villain_range->to_string();
		}
		to_string += "villain_range = " + villain_range_string + "\n";
	}
	if (is_in_hand)
	{
		to_string += "current_bet = ";
		to_string += std::to_string(current_bet);
		to_string += "\n";
	}
	return to_string;
}
