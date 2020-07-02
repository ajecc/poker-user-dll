#include "decision.h"
#include "poker_exception.h"
#include "hero_preflop_range.h"
#include "debug.h"
#include "villain_range.h"
#include "util.h"
#include "board_derived_info.h"
#include "prwin_calc.h"
#include <cassert>

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


static void 
sanitize_decision(decision_t* decision, board_t* board);

static float
calc_pot_odds(float pot, float call_sum);

static bool
is_appropriate_pot_odds_call(float pot_odds, float prwin);


static bool
is_appropriate_implied_odds_call(float pot, float call_sum, board_t* board,
									int hero_draws_count);



decision_t 
take_decision_preflop(player_t* hero, board_t* board)
{
	LOG_F(INFO, "started taking PREFLOP decision");
	auto* board_derived_info = board->board_derived_info;
	hero->hero_preflop_range = get_hero_preflop_range(
		hero->position,
		board_derived_info->main_villain->position,
		board_derived_info->bet_type
	);
	LOG_F(INFO, "got range");
	if (!hero->hero_preflop_range->contains(hero->hand))
	{
		return decision_t{ FOLD, 0 };
	}
	hero_preflop_range_hand_t range_hand = *hero->hero_preflop_range->fetch(hero->hand);
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
			to_raise_sum = 1.2f * board->pot; 
		}
	}
	return decision_t{ range_hand.hand_action, to_raise_sum };
}


decision_t 
take_decision_postflop(player_t* hero, board_t* board)
{
	LOG_F(INFO, "started taking POSTFLOP decision");
	// TODO: add implied odds into consideration 
	auto* board_derived_info = board->board_derived_info;
	float prwin = 1;
	float calling_rate = 0;
	for(const auto* villain : board->current_hand_players)
	{
		if (villain->is_hero)
		{
			continue;
		}
		prwin *= calc_prwin_vs_villain_range(hero->hand, villain->villain_range, board);
		calling_rate += calc_calling_rate_vs_villain_range(hero->hand, villain->villain_range,
			board, board_derived_info->bet_type);
	}
	assert(board->current_hand_players.size() >= 2);
	calling_rate /= (float)(board->current_hand_players.size() - 1);
	calling_rate *= (1 + prwin);
	const float BET_SIZE = board->pot * std::min(0.8f, calling_rate);
	LOG_F(INFO, "prwin = %f", prwin);
	LOG_F(INFO, "calling_rate = %f", calling_rate);
	// NOTE: the first member of the addition gurantees that we have better pot odds than the villains
	// the second part => profit taking and rake beating
	float raise_size = board->big_blind_sum;
	float pot_odds = calc_pot_odds(board->pot, board_derived_info->call_ammount);
	LOG_F(INFO, "pot_odds = %f", pot_odds);
	if (board_derived_info->bet_type == OPEN)
	{
		if (calling_rate < 0.4f)
		{
			return { CHECK, 0 };
		}
		else
		{
			// TODO: add something for TURN as well
			if ((board->stage == FLOP || board->stage == TURN) && is_board_wet_flop(board))
			{
				raise_size = 0.8f * board->pot;
			}
			else
			{
				raise_size = BET_SIZE;
			}
			return { RAISE, raise_size };
		}
	}
	else if (board_derived_info->bet_type == FACING_RAISE)
	{
		if (calling_rate > 0.4f)
		{
			return { RAISE, BET_SIZE };
		}
		else if (prwin > pot_odds)
		{
			return { CALL, 0 };
		}
		else
		{
			return { FOLD, 0 };
		}
	}
	else if (board_derived_info->bet_type == FACING_3BET ||
		board_derived_info->bet_type == FACING_4BET)
	{
		// TODO: this seems somewhat wrong. check it
		if (calling_rate > 0.6f)
		{
			return { RAISE, hero->balance };
		}
		if (prwin > pot_odds)
		{
			return { CALL, 0 };
		}
		else
		{
			return { FOLD, 0 };
		}
	}
	throw poker_exception_t("take_decision_postflop: some case was not handled");
}


decision_t
take_decision(player_t* hero, board_t* board)
{
	LOG_F(INFO, "started taking decision");
	decision_t decision;
	if (board->stage == PREFLOP)
	{
		decision = take_decision_preflop(hero, board);
	}
	else
	{
		decision = take_decision_postflop(hero, board);
	}
	sanitize_decision(&decision, board);
	if (decision.action == FOLD)
	{
		board->stage = INVALID_BOARD_STAGE;
	}
	return decision;
}


std::string
decision_t::to_string()
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


static void 
sanitize_decision(decision_t* decision, board_t* board)
{
	switch (decision->action)
	{
	case FOLD:
		if (FP_ARE_EQUAL(board->board_derived_info->current_bet, 0))
		{
			decision->action = CHECK;
		}
		break;
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


static float
calc_pot_odds(float pot, float call_sum)
{

	return call_sum / (call_sum + pot);
}


static bool
is_appropriate_pot_odds_call(float pot_odds, float prwin)
{
	return prwin > pot_odds;
}


static bool
is_appropriate_implied_odds_call(float pot, float call_sum, board_t* board,
									int hero_draws_count)
{
	// TODO: tweak this
	assert(board->stage == FLOP || board->stage == TURN);
	if (hero_draws_count == 0)
	{
		return false;
	}
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

	// NOTE: 0.33/0.7 are constants chosen by my instinc. Might need to tweak it
	if(board->stage == FLOP)
	{
		return (sum_needed_to_break_even < ((double)pot + call_sum) * 0.70 &&
			sum_needed_to_break_even < max_value);
	}
	return (sum_needed_to_break_even < ((double)pot + call_sum) * 0.70 &&
		sum_needed_to_break_even < max_value);
}
