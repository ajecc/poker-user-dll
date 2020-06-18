#include "board.h"
#include "poker_exception.h"
#include "villain_range.h"
#include "open_holdem_functions.h"
#include "debug.h"
#include "board_derived_info.h"
#include "util.h"
#include <cassert>
#include <algorithm>


static void 
clear_board(board_t* board);

static void
update_cards(board_t* board);

static void
update_players(board_t* board);

static void
update_player_positions(board_t* board);

static void
update_current_hand_players(board_t* board);

static void
update_big_blind_sum(board_t* board);

static void
update_pot(board_t* board);

static void
update_hero(board_t* board);

static void
update_board_derived_info(board_t* board, const board_stage_t& prev_stage);

static void
update_villain_actions(board_t* board, const board_stage_t& prev_stage);

static void
update_villain_ranges(const board_t* board);


board_t*
create_board()
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


void
destroy_board(board_t** board)
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


player_t*
get_player_by_label(board_t* board, const std::string& label)
{
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


void
update_board(board_t* board)
{
	auto prev_stage = board->stage;
	clear_board(board);
	update_cards(board);
	update_players(board);
	update_player_positions(board);
	update_current_hand_players(board);
	update_big_blind_sum(board);
	update_pot(board);
	update_hero(board);
	update_board_derived_info(board, prev_stage);
	update_villain_actions(board, prev_stage);
	update_villain_ranges(board);
	LOG_F(INFO, board->to_string().c_str());
}


player_t* 
get_next_player(board_t* board, player_t* player)
{
	std::string label = player->label;
	label[1] = (label[1] - '0' + 1) % PLAYERS_COUNT + '0';
	return get_player_by_label(board, label);
}


player_t*
get_next_player_in_game(board_t* board, player_t* player)
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


player_t*
get_next_player_in_hand(board_t* board, player_t* player)
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


static void
clear_board(board_t* board)
{
	if (nullptr == board)
	{
		throw poker_exception_t("Board should never be nullptr");
	}
	board->cards.clear();
	board->current_hand_players.clear();
}


static void
update_cards(board_t* board)
{
	std::string cards_query_string = "c0cardfaceX";
	for (int i = 0; ;i++)
	{
		cards_query_string[cards_query_string.size() - 1] = (char)(i + '0');
		std::string card_str = scrape_table_map_region(cards_query_string);
		if (card_str.empty())
		{
			break;
		}
		const card_t* card = get_card(card_str);
		board->cards.emplace_back(card);
	}
	std::sort(all(board->cards), card_ptr_cmp);
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
		throw poker_exception_t("Can't determine board stage...");
	}
}


static void
update_players(board_t* board)
{
	std::string player_label = "pX";
	for (char i = '0'; i < '0' + PLAYERS_COUNT; i++)
	{
		player_label[1] = i;
		player_t* player = get_player_by_label(board, player_label);
		update_player(player);
	}
}


static void
update_player_positions(board_t* board)
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
	current_player->in_front = in_front_cnt;

	player_t* stopping_point = current_player;
	for(;;)
	{
		DLOG(INFO, "pos label = %s", current_player->label.c_str());
		player_t* next_player = get_next_player_in_hand(board, current_player);
		if (next_player == stopping_point)
		{
			break;
		}
		next_player->in_front = current_player->in_front - 1;
		current_player = next_player;
	} 
	if (board->stage == PREFLOP)
	{
		small_blind->position = SB;
		big_blind->position = BB;
		if (dealer->is_small)
		{
			return;
		}
		dealer->position = BTN;
		player_t* utg = get_next_player_in_game(board, big_blind);
		player_t* hj = get_next_player_in_game(board, utg);
		player_t* co = get_next_player_in_game(board, hj);
		if (utg == dealer)
		{
			return;
		}
		if (hj == dealer)
		{
			utg->position = CO;
			return;
		}
		if (co == dealer)
		{
			utg->position = HJ;
			hj->position = CO;
			return;
		}
		utg->position = UTG;
		hj->position = HJ;
		co->position = CO;
	}
}


static void
update_current_hand_players(board_t* board)
{
	std::vector<player_t*> all_players = board->players;
	if (board->stage == PREFLOP)
	{
		std::sort(all(all_players), [](const player_t* lhs, const player_t* rhs)
			{
				return lhs->position < rhs->position;
			});
	}
	else
	{
		std::sort(all(all_players), [](const player_t* lhs, const player_t* rhs)
			{
				int lhs_position = (int)lhs->position;
				int rhs_position = (int)rhs->position;
				static const auto update_val = [](int* to_update)
				{
					if (*to_update == (int)SB)
					{
						*to_update = -(int)POSITION_COUNT;
					}
					else if (*to_update == (int)BB)
					{
						*to_update = -(int)POSITION_COUNT + 1;
					}
				};
				update_val(&lhs_position);
				update_val(&rhs_position);
				return lhs_position < rhs_position;
			});
	}
	for (auto* player : all_players)
	{
		if (player->is_in_hand)
		{
			board->current_hand_players.emplace_back(player);
		}
	}
}


static void 
update_big_blind_sum(board_t* board)
{
	board->big_blind_sum = (float)GetSymbol("bblind");
}


static void
update_pot(board_t* board)
{
	board->pot = (float)GetSymbol("pot");
}


static void
update_hero(board_t* board)
{
	board->hero = get_player_by_label(board, "p2");
	board->hero->is_hero = true;
}


static void
update_board_derived_info(board_t* board, const board_stage_t& prev_stage)
{
	if (prev_stage != board->stage)
	{
		delete board->board_derived_info;
		board->board_derived_info = nullptr;
	}
	auto* board_derived_info = new board_derived_info_t;
	*board_derived_info = get_board_derived_info(board->hero, board);
	if (board->board_derived_info != nullptr)
	{
		delete board->board_derived_info;
	}
	board->board_derived_info = board_derived_info;
	LOG_F(INFO, "BOARD_DERIVED_INFO:\n%s", 
		board_derived_info->to_string().c_str());
}


static void
update_villain_actions(board_t* board, const board_stage_t& prev_stage)
{
	// NOTE: this works because we only limp on SB
	auto villains_before_hero = board->board_derived_info->villains_before_hero;
	auto villains_after_hero = board->board_derived_info->villains_after_hero;
	float hero_current_bet = board->hero->current_bet;
	float last_bet = 0;
	bool opened = false;
	if (prev_stage != board->stage)
	{
		for (auto* player : villains_before_hero)
		{
			if (FP_ARE_EQUAL(player->current_bet, board->big_blind_sum) && 
				board->stage == PREFLOP)
			{
				player->villain_action = VILLAIN_ACTION_LIMP;
			}
			else if (FP_ARE_EQUAL(player->current_bet, 0))
			{
				player->villain_action = VILLAIN_ACTION_CHECK;
			}
			else 
			{
				if (player->current_bet > last_bet && !opened)
				{
					player->villain_action = VILLAIN_ACTION_OPEN;
					opened = true;
				}
				else if (player->current_bet > last_bet && opened)
				{
					player->villain_action = VILLAIN_ACTION_RAISE;
				}
				else
				{
					player->villain_action = VILLAIN_ACTION_CALL;
				}
				last_bet = player->current_bet;
			}
		}
		for (auto* player : villains_after_hero)
		{
			if (player->is_big && board->stage == PREFLOP)
			{
				player->villain_action = VILLAIN_ACTION_CHECK;
			}
			else
			{
				// TODO: this might be check postflop, depending on the decision we make.
				player->villain_action = VILLAIN_ACTION_CALL;
			}
		}
	}
	else
	{
		if (FP_ARE_DIFFERENT(board->hero->current_bet, 0.0f))
		{
			opened = true;
			last_bet = board->hero->current_bet;
		}
		bool raised = false;
		for (const auto* player : board->current_hand_players)
		{
			if (player->current_bet > 0 &&
				FP_ARE_DIFFERENT(board->hero->current_bet, last_bet))
			{
				raised = true;
				break;
			}
		}
		auto players = concat(villains_after_hero, villains_before_hero);
		for (auto* player : players)
		{
			if (raised)
			{
				player->villain_action = VILLAIN_ACTION_RAISE;
				continue;
			}
			if (opened)
			{
				if (player->current_bet > last_bet)
				{
					raised = true;
					player->villain_action = VILLAIN_ACTION_RAISE;
				}
				else
				{
					player->villain_action = VILLAIN_ACTION_CALL;
				}
			}
			else
			{
				if (player->current_bet > 0)
				{
					player->villain_action = VILLAIN_ACTION_OPEN;
					opened = true;
				}
				else
				{
					player->villain_action = VILLAIN_ACTION_CHECK;
				}
			}
		}
	}
}


static void
update_villain_ranges(const board_t* board)
{
	for (auto* player : board->current_hand_players)
	{
		if (!player->is_hero)
		{
			update_player_villain_range(player, board);
		}
	}
}


std::string 
board_t::to_string()
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
	to_string += "HERO: " + hero->hand->to_string() + "\n";
	to_string += "CURRENT PLAYERS CNT: " + std::to_string(current_hand_players.size());
	return to_string;
}
