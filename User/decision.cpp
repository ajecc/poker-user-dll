#include "decision.h"
#include "poker_exception.h"
#include "range.h"
#include "debug.h"
#include "util.h"
#include "board_derived_info.h"
#ifdef min
#undef min
#endif

// TODO: find appropriate values for these
#define TRAP_PRWIN 0.93f
#define RAISE_PRWIN 0.84f
#define RAISE_TEAK (-0.1f)
#define BET_WETBOARD_PRWIN 0.65f
#define BET_WETBOARD_TWEAK (0.0f)

#define CBET_PRWIN_OUT_POSITION 0.45f
#define CBET_OUT_POSITION_TWEAK 0.1f
#define CBET_PRWIN_IN_POSITION 0.39f
#define CBET_IN_POSITION_TWEAK 0.1f
#define MAX_CBET_SUM 0.65f

#define CALL_PRWIN_EXPLOIT_TEAK (0.06f)
#define ENEMY_CBET_TWEAK (-0.05f)
#define ALLIN_ON_3BET 0.934


static bool is_board_wet_flop(board_t* board);


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
	float prwin_raw = (float)pow(
		calc_prwin_vs_any_hand(hero->hand, board),
		board->current_hand_players.size() - 1
	);
	if (board->board_derived_info->bet_type == OPEN)
	{
		if (prwin_raw > TRAP_PRWIN)
		{
			return { CHECK, 0 };
		}
		if (is_board_wet_flop(board))
		{
			if (prwin_raw > BET_WETBOARD_PRWIN)
			{
				return { RAISE, board->pot * (prwin_raw + BET_WETBOARD_TWEAK) };
			}
			return { CHECK, 0 };
		}
		else if (!board->board_derived_info->villains_after_hero.empty())
		{
			if (prwin_raw > CBET_PRWIN_OUT_POSITION)
			{
				return { RAISE, board->pot * (prwin_raw + CBET_OUT_POSITION_TWEAK) };
			}
			return { CHECK, 0 };
		}
		else
		{
			if (prwin_raw > CBET_PRWIN_IN_POSITION)
			{
				float bet_ammount = prwin_raw + CBET_OUT_POSITION_TWEAK;
				bet_ammount = std::min(bet_ammount, MAX_CBET_SUM);
				return { RAISE, board->pot * bet_ammount };
			}
			return { CHECK, 0 };
		}
	}
	else if (board->board_derived_info->bet_type == FACING_RAISE)
	{
		if (prwin_raw > TRAP_PRWIN)
		{
			return { CALL, 0 };
		}
		if (prwin_raw > RAISE_PRWIN)
		{
			if (generate_random() < 0.2)
			{
				return { CALL, 0 };
			}
			return { RAISE, board->pot * (prwin_raw + RAISE_TEAK) };
		}
		float prwin_needed_to_call =
			board->board_derived_info->current_bet / board->pot +
			CALL_PRWIN_EXPLOIT_TEAK;
		float prwin_tweaked = prwin_raw;
		prwin_tweaked += ENEMY_CBET_TWEAK;
		// TODO: add support for potential value from draws
		if (prwin_tweaked > prwin_needed_to_call)
		{
			return { CALL, 0 };
		}
	}
	else if (board->board_derived_info->bet_type == FACING_3BET ||
		board->board_derived_info->bet_type == FACING_4BET)
	{
		if (prwin_raw > ALLIN_ON_3BET)
		{
			return { RAISE, hero->balance };
		}
		// TDOO: maybe call sometimes
		return { FOLD, 0 };
	}
	throw poker_exception_t("take_decision_flop: some case was not handled");
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
			if (contains(board_derived_info->villains_before_hero, board_derived_info->main_villain))
			{
				board->board_derived_info->bet_type = FACING_3BET;
			}
			else
			{
				board->board_derived_info->bet_type = FACING_4BET;
			}
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


static bool is_board_wet_flop(board_t* board)
{
	return board->board_derived_info->villain_draws_flop.size() > 7;
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

