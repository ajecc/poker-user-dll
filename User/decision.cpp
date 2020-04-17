#include "decision.h"
#include "poker_exception.h"
#include "range.h"
#include "debug.h"
#include "util.h"

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
	board_derived_info.main_villain = nullptr;
	board_derived_info.secondary_villain = nullptr;
	board_derived_info.bet_type = OPEN;

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
			board_derived_info.villains_before_hero.emplace_back(villain);
		}
		else
		{
			board_derived_info.villains_after_hero.emplace_back(villain);
		}
	}
	for (auto* villain : board_derived_info.villains_after_hero)
	{
		if (FP_ARE_DIFFERENT(current_bet, villain->current_bet))
		{
			current_bet = villain->current_bet;
			++board_derived_info.bet_type;
		}
		if (current_bet != 0)
		{
			board_derived_info.secondary_villain = board_derived_info.main_villain;
			board_derived_info.main_villain = villain;
		}
	}
	for (auto* villain : board_derived_info.villains_before_hero)
	{
		if (FP_ARE_DIFFERENT(current_bet, villain->current_bet))
		{
			current_bet = villain->current_bet;
			++board_derived_info.bet_type;
		}
		board_derived_info.secondary_villain = board_derived_info.main_villain;
		board_derived_info.main_villain = villain;
	}
	board_derived_info.current_bet = current_bet;
	return board_derived_info;
}


decision_t take_decision_preflop(player_t* hero, board_t* board)
{
	DLOG(INFO, "started taking PREFLOP decision");
	board_derived_info_t* board_derived_info = board->board_derived_info;
	hero->range = copy_range(get_range(
		hero->position,
		board_derived_info->main_villain->position,
		board_derived_info->bet_type
	));
	if (!hero->range->contains(hero->hand))
	{
		return decision_t{ FOLD, 0 };
	}
	range_hand_t range_hand = *hero->range->fetch(hero->hand);
	apply_raise_prob(&range_hand);
	float to_raise_sum = 0;
	if (range_hand.hand_action == RAISE)
	{
		if (board_derived_info->bet_type == FACING_4BET)
		{
			to_raise_sum = hero->balance;
		}
		else if (board_derived_info->bet_type == OPEN)
		{
			// bet 3 + number of limpers BBs
			to_raise_sum = board->big_blind_sum * 
				(3 + board_derived_info->villains_before_hero.size());
		}
		else
		{
			// let's say we bet 65% of the pot otherwise
			to_raise_sum = 0.65f * board->pot; 
		}
	}
	return decision_t{ range_hand.hand_action, to_raise_sum };
}


decision_t take_decision_flop(player_t* hero, board_t* board)
{
	DLOG(INFO, "started taking FLOP decision");
	return {FOLD, 0};
}


decision_t take_decision_turn(player_t* hero, board_t* board)
{
	DLOG(INFO, "started taking TURN decision");
	return {FOLD, 0};
}


decision_t take_decision_river(player_t* hero, board_t* board)
{
	DLOG(INFO, "started taking RIVER decision");
	return {FOLD, 0};
}


decision_t take_decision(player_t* hero, board_t* board)
{
	board_derived_info_t* board_derived_info = new board_derived_info_t;
	*board_derived_info = get_board_derived_info(hero, board);
	if (board->board_derived_info != nullptr)
	{
		if (board->board_derived_info->bet_type >= FACING_RAISE)
		{
			++board_derived_info->bet_type;
		}
		if (board->board_derived_info->bet_type >= FACING_3BET)
		{
			++board_derived_info->bet_type;
		}
		delete board->board_derived_info;
	}
	board->board_derived_info = board_derived_info;

	switch (board->stage)
	{
	case PREFLOP:
		return take_decision_preflop(hero, board);
	case FLOP:
		return take_decision_flop(hero, board);
	case TURN:
		return take_decision_turn(hero, board);
	case RIVER:
		return take_decision_river(hero, board);
	default:
		throw poker_exception_t("take_decision: invalid board->stage");
	}
}


std::string decision_t::to_string()
{
	std::string res = "";
	switch (action)
	{
	case FOLD:
		res += "FOLD, ";
		break;
	case CHECK:
		res += "CHECK, ";
		break;
	case CALL:
		res += "CALL, ";
		break;
	case RAISE:
		res += "RAISE, ";
		break;
	default:
		throw poker_exception_t("decision::to_string: invalid action");
	}
	res += std::to_string(sum);
	return res;
}
