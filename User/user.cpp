#include "clean_windows.h"
#include <conio.h>
#include <cstdio>
#include <chrono>
#include <random>
#include <excpt.h>
#include <map>
#include <sstream>
#include "loguru.h"
#include "open_holdem_functions.h"
#include "debug.h"
#include "board.h"
#include "hand.h"
#include "hero_preflop_range.h"
#include "hand_board_result.h"
#include "decision.h"
#include "poker_exception.h"
#include "prwin_calc.h"
#include "villain_range.h"


// GLOBALS
std::map<std::string, std::string> g_symbols;

board_t* g_board;

std::vector<const card_t*> g_all_cards;
std::vector<const hand_t*> g_all_hands;

std::vector<const hero_preflop_range_t*> g_open_hero_preflop_ranges, g_facing_raise_hero_preflop_ranges,
						g_facing_3bet_hero_preflop_ranges, g_facing_4bet_hero_preflop_ranges;

const hand_board_result_t* g_all_hand_board_results;

std::vector<float> g_all_prwin_vs_any_hand_flop;

std::vector<const villain_range_t*> g_open_villain_preflop_range, g_call_villain_preflop_range;
const villain_range_t* g_limp_villain_preflop_range;
const villain_range_t* g_check_villain_preflop_range;
const villain_range_t* g_raise_villain_preflop_range;


void 
create_globals()
{
	g_board = create_board();
	g_all_cards = create_all_cards();
	g_all_hands = create_all_hands();

	g_open_hero_preflop_ranges = create_open_hero_preflop_ranges();
	g_facing_raise_hero_preflop_ranges = create_facing_raise_hero_preflop_ranges();
	g_facing_3bet_hero_preflop_ranges = create_facing_3bet_hero_preflop_ranges();
	g_facing_4bet_hero_preflop_ranges = create_facing_4bet_hero_preflop_ranges();

	g_all_hand_board_results = create_all_hand_board_results();

	g_all_prwin_vs_any_hand_flop = create_all_prwin_vs_any_hand_flop();

	g_open_villain_preflop_range = create_open_villain_preflop_range();
	g_call_villain_preflop_range = create_call_villain_preflop_range();
	g_limp_villain_preflop_range = create_limp_villain_preflop_range();
	g_check_villain_preflop_range = create_check_villain_preflop_range();
	g_raise_villain_preflop_range = create_raise_villain_preflop_range();

	LOG_F(INFO, "Created all globals successfully");
}


extern "C" __declspec(dllexport) int __stdcall
update_symbols(const char* psymbols)
{
	if (psymbols == nullptr)
	{
		return -1;
	}
	LOG_F(INFO, psymbols);
	std::string symbols = std::string(psymbols);
	std::string key, val;
	std::istringstream iss(symbols);
	while (std::getline(std::getline(iss, key, ':') >> std::ws, val))
	{
		g_symbols[key] = val;
	}
	return 0;
}


extern "C" __declspec(dllexport) double __stdcall
process_query(const char* pquery)
{
	if (pquery == nullptr)
	{
		return -1;
	}
	std::string query = std::string(pquery);
	static decision_t decision = {};
	static int consec_exception_count = 0;
	if (query == "dll$beep")
	{
		try
		{
			auto start_time = std::chrono::steady_clock::now();
			update_board(g_board);
			decision = take_decision(g_board->hero, g_board);
			auto end_time = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsed_time = end_time - start_time;
			LOG_F(INFO, decision.to_string().c_str());
			LOG_F(INFO, "Time to take the decision = %fs\n", elapsed_time.count());
			consec_exception_count = 0;
		}
		catch(poker_exception_t& e)
		{
			LOG_F(INFO, "Exception encountered: %s", e.what());
			consec_exception_count++;
			if (consec_exception_count >= 12)
			{
				LOG_F(FATAL, "Too many consecutive excetions encountered");
			}
			decision.action = FOLD;
		}
		return 0;
	}
	else if (query == "dll$check")
	{
		return (decision.action == CHECK || decision.action == FOLD);
	}
	else if (query == "dll$call")
	{
		return decision.action == CALL;
	}
	else if (query == "dll$bet")
	{
		return decision.action == RAISE;
	}
	else if (query == "dll$raise")
	{
		return decision.action == RAISE;
	}
	else if (query == "dll$betsize")
	{
		DLOG(INFO, "in dll$betsize (%f)", decision.sum);
		if (decision.action == CHECK || 
			decision.action == FOLD ||
			decision.action == CALL)
		{
			decision.sum = 0;
		}
		return decision.sum;
	}
	LOG_F(FATAL, "invalid query: %s", pquery);
	return -1;
}
		

// TODO: msg larisa dupa ce termin
BOOL APIENTRY
DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) 
{
	FILE* conout = nullptr;
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		init_log(&conout);
		create_globals();
#ifdef _DEBUG
		LOG_F(INFO, "Debug build");
#else
		LOG_F(INFO, "Release build");
#endif
		LOG_F(INFO, "Compile time: " __TIME__ " -- " __DATE__);
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		// TODO: add something to destroy everything
		destroy_board(&g_board);
		uninit_log(conout);
	}
	return TRUE;
} 
