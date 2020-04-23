// NOTE: Required OpenHoldem version: 12.1.6 ?? 

// Needs to be defined here, before #include "user.h"
// to generate proper export- and inport-definitions
#define USER_DLL

#include <conio.h>
#include <Windows.h>
#include <cstdio>
#include <random>
#include <excpt.h>
#include "loguru.h"
#include "user.h"
#include "open_holdem_functions.h"
#include "debug.h"
#include "board.h"
#include "hand.h"
#include "range.h"
#include "hand_board_result.h"
#include "decision.h"

void __stdcall DLLUpdateOnNewFormula() {}
void __stdcall DLLUpdateOnConnection() {}
void __stdcall DLLUpdateOnHandreset () {}
void __stdcall DLLUpdateOnNewRound  () {}
void __stdcall DLLUpdateOnMyTurn    () {}
void __stdcall DLLUpdateOnHeartbeat () {}


// GLOBALS
board_t* g_board;
std::vector<const card_t*> g_all_cards;
std::vector<const hand_t*> g_all_hands;
std::vector<const range_t*> g_open_ranges, g_facing_raise_ranges, g_facing_3bet_ranges, g_facing_4bet_ranges;
const hand_board_result_t* g_all_hand_board_results;


void create_globals()
{
	g_board = create_board();
	g_all_cards = create_all_cards();
	g_all_hands = create_all_hands();

	g_open_ranges = create_open_ranges();
	g_facing_raise_ranges = create_facing_raise_ranges();
	g_facing_3bet_ranges = create_facing_3bet_ranges();
	g_facing_4bet_ranges = create_facing_4bet_ranges();

	g_all_hand_board_results = create_all_hand_board_results();
	LOG_F(INFO, "Created all globals successfully");
}


// Handling the lookup of dll$symbols
DLL_IMPLEMENTS double __stdcall ProcessQuery(const char* pquery)
{
	if (nullptr == pquery)
	{
		return -1;
	}
	std::string query = std::string(pquery);
	static decision_t decision = {};
	if (query == "dll$beep")
	{
		try
		{
			update_board(g_board);
			decision = take_decision(g_board->hero, g_board);
			LOG_F(INFO, decision.to_string().c_str());
		}
		catch(std::exception& e)
		{
			LOG_F(FATAL, e.what());
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
		LOG_F(INFO, "in dll$betsize (%f)", decision.sum);
		if (g_board->stage == PREFLOP)
		{
			// NOTE: I have no idea why, but OpenHoldem bets 1 blind less than I tell it to??
			decision.sum += g_board->big_blind_sum;
		}
		return decision.sum;
	}
	LOG_F(FATAL, "invalid query: %s", pquery);
	return -1;
}


// TODO: msg larisa dupa ce termin
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) 
{
	FILE* conout = nullptr;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			//__debugbreak();
			InitializeOpenHoldemFunctionInterface();
			init_log(&conout);
			create_globals();
		} break;

		case DLL_PROCESS_DETACH:
		{
			// TODO: add something to destroy everything
			destroy_board(&g_board);
			uninit_log(conout);
		} break;

		default:
		{
		}
	}
	return TRUE;
} 
