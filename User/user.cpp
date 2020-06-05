// NOTE: Required OpenHoldem version: 12.1.6 

// NOTE: this needs to be at the top
#define USER_DLL 

#include <conio.h>
#include <Windows.h>
#include <cstdio>
#include <random>
#include <excpt.h>
#include <filesystem>
#include <future>
#include "loguru.h"
#include "user.h"
#include "open_holdem_functions.h"
#include "debug.h"
#include "board.h"
#include "hand.h"
#include "range.h"
#include "hand_board_result.h"
#include "decision.h"
#include "poker_exception.h"

void __stdcall
DLLUpdateOnNewFormula() {}

void __stdcall
DLLUpdateOnConnection() {}

void __stdcall
DLLUpdateOnHandreset() {}

void __stdcall
DLLUpdateOnNewRound() {}

void __stdcall
DLLUpdateOnMyTurn() {}

void __stdcall
DLLUpdateOnHeartbeat() {}


// GLOBALS
board_t* g_board;
std::vector<const card_t*> g_all_cards;
std::vector<const hand_t*> g_all_hands;
std::vector<const range_t*> g_open_ranges, g_facing_raise_ranges,
							g_facing_3bet_ranges, g_facing_4bet_ranges;
const hand_board_result_t* g_all_hand_board_results;


void 
create_globals()
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
DLL_IMPLEMENTS double __stdcall
ProcessQuery(const char* pquery)
{
	if (nullptr == pquery)
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
			update_board(g_board);
			decision = take_decision(g_board->hero, g_board);
			LOG_F(INFO, (decision.to_string() + "\n").c_str());
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
		

#ifdef _WINDLL
// TODO: msg larisa dupa ce termin
BOOL APIENTRY
DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) 
{
	FILE* conout = nullptr;
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		InitializeOpenHoldemFunctionInterface();
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
#else
#include <fstream>
#include <iostream>
size_t i, j, k, l, m;
std::vector<float> results;
bool done = false;

void save_state()
{
	std::cout << "saving state...\n";
	std::ofstream saved_stream_state("saved_state.txt");
	saved_stream_state << i << ' ' << j << ' ' << k << ' ' << l << ' ' << m + 1;
	std::ofstream prwin_results;
	prwin_results.open("prwin_results_any_hand.cache", 
		std::ofstream::app | std::ofstream::binary);
	prwin_results.write((char*)&results[0], sizeof(float) * results.size());
	std::cout << "successfully saved state...\n";
	prwin_results.clear();
}


BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	done = true;
	Sleep(50000);
	return TRUE;
}


int main()
{
	auto file_size = (float)std::filesystem::file_size(
					"prwin_results_any_hand.cache"); 
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);
	FILE* conout = nullptr;
	init_log(&conout);
	create_globals();
	std::ifstream saved_state_stream("saved_state.txt");
	saved_state_stream >> i >> j >> k >> l >> m;
	saved_state_stream.close();
	if (i == g_all_cards.size())
	{
		i = 0;
	}
	for (i; i < g_all_cards.size(); i++)
	{
		if (j <= i || j == g_all_cards.size())
		{
			j = i + 1;
		}
		for (j; j < g_all_cards.size(); j++)
		{
			if (k == g_all_cards.size())
			{
				k = 0;
			}
			for (k; k < g_all_cards.size(); k++)
			{
				if (l <= k || l == g_all_cards.size())
				{
					l = k + 1;
				}
				for (l; l < g_all_cards.size(); l++)
				{
					if (m <= l || m == g_all_cards.size())
					{
						m = l + 1;
					}
					for (m; m < g_all_cards.size(); m++)
					{
						if (k == j || l == j || m == j ||
							k == i || l == i || m == i)
						{
							continue;
						}
						const hand_t* hero_hand = 
							get_hand(g_all_cards[i], g_all_cards[j]);
						board_t board;
						board.cards = { g_all_cards[k], g_all_cards[l], g_all_cards[m] };
						results.push_back(calc_prwin_vs_any_hand(hero_hand, &board));
						if (done)
						{
							save_state();
							MessageBox(NULL, "Saved, everything ok", "OK", MB_OK);
							Sleep(50000);
							exit(0);
						}
					}
				}
			}
			float progress = (file_size + results.size() * sizeof(float)) / 1039584;
			std::cout << "[#] Done: " << progress << "%\n";
		}
	}
	std::cout << "done!\n";
}
#endif
