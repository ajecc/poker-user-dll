#include "decision.h"
#include "poker_exception.h"
#include "range.h"
#include "debug.h"
#include "util.h"
#include "board_derived_info.h"
#ifdef min
#undef min
#undef max
#endif

// TODO: find appropriate values for these
#define STRONG_HAND_PRWIN 0.93f
// bet 30% of the time with a really strong hand
#define STRONG_HAND_BET_FREQ 0.3f  
#define RAISE_PRWIN 0.84f
#define RAISE_TEAK (-0.1f)
#define BET_WETBOARD_PRWIN 0.65f
#define BET_WETBOARD_TWEAK (0.0f)

#define CBET_PRWIN_OUT_POSITION 0.45f
#define CBET_OUT_POSITION_TWEAK 0.1f
#define CBET_PRWIN_IN_POSITION 0.39f
#define CBET_IN_POSITION_TWEAK 0.1f
#define MAX_CBET_SUM 0.65f
#define CBET_FREQ 0.7f

#define CALL_PRWIN_EXPLOIT_TEAK (0.06f)
#define ENEMY_CBET_TWEAK (-0.05f)
#define ALLIN_ON_3BET 0.934


static bool is_board_wet_flop(board_t* board);

static void sanitize_decision(decision_t* decision, board_t* board);

static float calc_pot_odds(float pot, float call_sum);

static bool is_appropriate_implied_odds_call(float pot, float call_sum, board_t* board, int hero_draws_count);


decision_t take_decision_preflop(player_t* hero, board_t* board)
{
	DLOG(INFO, "started taking PREFLOP decision");
	board_derived_info_t* board_derived_info = board->board_derived_info;
	hero->range = copy_range(get_range(
		hero->position,
		board_derived_info->main_villain->position,
		board_derived_info->bet_type
	));
	DLOG(INFO, "got range");
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
		if (prwin_raw > STRONG_HAND_PRWIN)
		{
			if (is_board_wet_flop(board))
			{
				return { RAISE, board->pot * 0.8f };
			}
			if (STRONG_HAND_BET_FREQ < generate_random())
			{
				return { RAISE, board->pot * 0.5f };
			}
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
			if (board->current_hand_players.size() == 2 && CBET_FREQ < generate_random())
			{
				return { RAISE, board->pot * 0.5f };
			}
			return { CHECK, 0 };
		}
	}
	else if (board->board_derived_info->bet_type == FACING_RAISE)
	{
		if (prwin_raw > STRONG_HAND_PRWIN)
		{
			if (STRONG_HAND_BET_FREQ < generate_random())
			{
				return { RAISE, board->pot };
			}
			return { CALL, 0 };
		}
		if (prwin_raw > calc_pot_odds(board->pot, board->board_derived_info->call_ammount))
		{
			// TODO: tweak this such that you call tighter
			return { CALL, 0 };
		}
		if (is_appropriate_implied_odds_call(
			board->pot,
			board->board_derived_info->call_ammount,
			board,
			board->board_derived_info->hero_draws_flop.size()))
		{
			return { CALL, 0 };
		}
		return { FOLD, 0 };
	}
	else if (board->board_derived_info->bet_type == FACING_3BET ||
		board->board_derived_info->bet_type == FACING_4BET)
	{
		if (prwin_raw > ALLIN_ON_3BET)
		{
			return { RAISE, hero->balance };
		}
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
		delete board->board_derived_info;
	}
	board->board_derived_info = board_derived_info;
	DLOG_F(INFO, "BOARD_DERIVED_INFO:\n%s", board_derived_info->to_string().c_str());

	decision_t decision;
	switch (board->stage)
	{
	case PREFLOP:
		decision = take_decision_preflop(hero, board);
		break;
	case FLOP:
		decision = take_decision_flop(hero, board);
		break;
	case TURN:
		decision = take_decision_turn(hero, board);
		break;
	case RIVER:
		decision = take_decision_river(hero, board);
		break;
	default:
		throw poker_exception_t("take_decision: invalid board->stage");
	}
	sanitize_decision(&decision, board);
	return decision;
}


static bool is_board_wet_flop(board_t* board)
{
	// TODO: give a better definition of a wet board
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


static void sanitize_decision(decision_t* decision, board_t* board)
{
	switch (decision->action)
	{
	case CHECK:
		if (FP_ARE_DIFFERENT(board->board_derived_info->current_bet, 0))
		{
			decision->action = FOLD;
		}
		break;
	case CALL:
		if (FP_ARE_EQUAL(board->board_derived_info->current_bet, 0) ||
			FP_ARE_EQUAL(board->board_derived_info->current_bet, board->hero->current_bet))
		{
			decision->action = CHECK;
		}
		break;
	case RAISE:
		decision->sum = std::max(decision->sum, board->big_blind_sum);
		decision->sum = std::min(decision->sum, board->hero->balance);
		// TODO: elaborate this
	}
}


static float calc_pot_odds(float pot, float call_sum)
{
	return call_sum / (call_sum + pot);
}


static bool is_appropriate_implied_odds_call(float pot, float call_sum, board_t* board, int hero_draws_count)
{
	float max_value = 0;
	for (auto* player : board->current_hand_players)
	{
		max_value += player->balance;
	}
	max_value -= board->hero->balance;
	float hit_draw_chance = (float)hero_draws_count / 
		(float)(NUMBER_OF_CARDS - 2 - board->cards.size());
	float sum_needed_to_break_even = (call_sum - hit_draw_chance * (pot + call_sum))
		/ hit_draw_chance;
	// NOTE: 0.33 is a constant chosen by my instinc. Might need to tweak it
	return (sum_needed_to_break_even < ((double)pot + call_sum) * 0.33 &&
		sum_needed_to_break_even < max_value);
}
