#include "board_derived_info.h"
#include "util.h"
#include "card.h"
#include <set>


extern std::vector<const card_t*> g_all_cards;


static void get_villains_info(player_t* hero, board_t* board, board_derived_info_t* board_derived_info);

static void get_villain_draws(board_t* board, board_derived_info_t* board_derived_info);


bet_type_t& operator++(bet_type_t& bet_type)
{
	if (bet_type == FACING_4BET)
	{
		return bet_type;
	}
	bet_type = (bet_type_t)((int)bet_type + 1);
	return bet_type;
}


board_derived_info_t get_board_derived_info(player_t* hero, board_t* board)
{
	// TODO: make sure the board is updated properly (by that I mean the order of the players)
	board_derived_info_t board_derived_info;
	get_villains_info(hero, board, &board_derived_info);
	if (board->stage == TURN)
	{
		board_derived_info.villain_draws_flop = board->board_derived_info->villain_draws_flop;
	}
	else if (board->stage == RIVER)
	{
		board_derived_info.villain_draws_flop = board->board_derived_info->villain_draws_flop;
		board_derived_info.villain_draws_turn = board->board_derived_info->villain_draws_turn;
	}
	if (board->stage == FLOP || board->stage == TURN)
	{
		get_villain_draws(board, &board_derived_info);
	}
	return board_derived_info;
}


std::vector<const card_t*> find_remaining_cards(const hand_t* hero, const hand_t* villain, board_t* board)
{
	std::vector<const card_t*> current_cards = board->cards;
	if (hero != nullptr)
	{
		current_cards.emplace_back(hero->cards[0]);
		current_cards.emplace_back(hero->cards[1]);
	}
	if (villain != nullptr)
	{
		current_cards.emplace_back(villain->cards[0]);
		current_cards.emplace_back(villain->cards[1]);
	}
	insertion_sort(&current_cards[0], current_cards.size(), 
		[](const card_t* lhs, const card_t* rhs) {return *lhs < *rhs; });

	std::vector<const card_t*> remaining_cards;
	size_t current_cards_ind = 0;
	for (size_t i = 0; i < g_all_cards.size(); i++)
	{
		if (current_cards_ind < current_cards.size() && 
			g_all_cards[i] == current_cards[current_cards_ind])
		{
			current_cards_ind++;
		}
		else
		{
			remaining_cards.emplace_back(g_all_cards[i]);
		}
	}
	return remaining_cards;
}


static void get_villains_info(player_t* hero, board_t* board, board_derived_info_t* board_derived_info)
{
	board_derived_info->main_villain = nullptr;
	board_derived_info->secondary_villain = nullptr;
	board_derived_info->bet_type = OPEN;

	bool after_hero = false;
	float current_bet = 0;
	for (auto* villain : board->current_hand_players)
	{
		if (villain->label == hero->label)
		{
			after_hero = true;
			continue;
		}
		if (!after_hero)
		{
			board_derived_info->villains_before_hero.emplace_back(villain);
		}
		else
		{
			board_derived_info->villains_after_hero.emplace_back(villain);
		}
	}
	for (auto* villain : board_derived_info->villains_after_hero)
	{
		if (FP_ARE_DIFFERENT(current_bet, villain->current_bet))
		{
			current_bet = villain->current_bet;
			board_derived_info->bet_type = FACING_RAISE;
		}
		if (current_bet != 0)
		{
			board_derived_info->secondary_villain = board_derived_info->main_villain;
			board_derived_info->main_villain = villain;
		}
	}
	for (auto* villain : board_derived_info->villains_before_hero)
	{
		if (FP_ARE_DIFFERENT(current_bet, villain->current_bet))
		{
			current_bet = villain->current_bet;
			board_derived_info->bet_type = FACING_RAISE;
		}
		board_derived_info->secondary_villain = board_derived_info->main_villain;
		board_derived_info->main_villain = villain;
	}
	board_derived_info->current_bet = current_bet;
	if (board_derived_info->bet_type == FACING_RAISE &&
		board->stage == PREFLOP &&
		board_derived_info->main_villain != nullptr &&
		board_derived_info->main_villain->current_bet == board->big_blind_sum)
	{
		board_derived_info->bet_type = OPEN;
	}
}


static void get_villain_draws(board_t* board, board_derived_info_t* board_derived_info)
{
	// TODO: maybe optimizee this
	std::set<const card_t*> draws;
	std::vector<const card_t*> cards = board->cards;
	std::vector<const card_t*> remaining_cards = find_remaining_cards(nullptr, nullptr, board);
	count_t colors_count[COLOR_COUNT] = { 0 };
	for (const auto* card : cards)
	{
		colors_count[(int)card->color]++;
	}
	for (color_t color = H; color <= S; ++color)
	{
		if (colors_count[(int)color] == 2 || colors_count[(int)color] == 3)
		{
			for (auto* remaining_card : remaining_cards)
			{
				if (remaining_card->color == color)
				{
					draws.insert(remaining_card);
				}
			}
		}
	}
	card_t small_aces[4];
	count_t aces_count = 0;
	for (const auto* card : board->cards)
	{
		if (card->rank == _A)
		{
			small_aces[aces_count] = card_t{ card->color, _A_1 };
			cards.push_back(&small_aces[aces_count]);
			aces_count++;
		}
	}
	std::sort(all(cards), [](const card_t* lhs, const card_t* rhs) {return *lhs < *rhs; });
	card_t invalid_card{ H, INVALID_RANK };
	cards.push_back(&invalid_card);
	count_t consec = 1;
	for (int i = 1; i < (int)cards.size(); i++)
	{
		if (cards[i]->rank == cards[i - 1]->rank + 1)
		{
			consec++;
		}
		else if (cards[i]->rank == cards[i - 1]->rank)
		{
			continue;
		}
		else if (consec == 2)
		{
			consec = 1;
			for (color_t color = H; color <= S; ++color)
			{
				draws.insert(get_card((rank_t)(cards[i - 1]->rank + 1), color));
				if (cards[i - 1]->rank - 2 >= _3)
				{
					draws.insert(get_card((rank_t)(cards[i - 1]->rank - 2), color));
				}
			}
		}
		else if (consec >= 3)
		{
			if (consec == 3)
			{
				for (color_t color = H; color <= S; ++color)
				{
					int rank = (rank_t)cards[i - 1]->rank - 3;
					if (rank == 1)
					{
						rank = _A;
					}
					draws.insert(get_card((rank_t)rank, color));
					rank--;
					if (rank == 1)
					{
						rank = _A;
					}
					draws.insert(get_card((rank_t)rank, color));
				}
			}
			int rank = cards[i - 1]->rank + 1;
			if (rank > _A)
			{
				continue;
			}
			for (color_t color = H; color <= S; ++color)
			{
				draws.insert(get_card((rank_t)rank, color));
			}
			rank++;
			if (rank > _A)
			{
				continue;
			}
			for (color_t color = H; color <= S; ++color)
			{
				draws.insert(get_card((rank_t)rank, color));
			}
		}
	}

	std::vector<const card_t*>& to_add =
		(board->stage == FLOP ?
		board_derived_info->villain_draws_flop :
		board_derived_info->villain_draws_turn);
	for (auto* card : cards)
	{
		to_add.push_back(card);
	}
}
