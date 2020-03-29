// NOTE: Required OpenHoldem version: 12.1.6 ?? 

// Needs to be defined here, before #include "user.h"
// to generate proper export- and inport-definitions
#define USER_DLL

#include <conio.h>
#include <Windows.h>
#include <cstdio>
#include <random>
#include "loguru.h"
#include "user.h"
#include "open_holdem_functions.h"
#include "debug.h"
#include "board.h"
#include "hand.h"
#include "range.h"



void __stdcall DLLUpdateOnNewFormula()
{
}


void __stdcall DLLUpdateOnConnection()
{
}


void __stdcall DLLUpdateOnHandreset()
{
}


void __stdcall DLLUpdateOnNewRound()
{
}


void __stdcall DLLUpdateOnMyTurn()
{
}


void __stdcall DLLUpdateOnHeartbeat() 
{
}

board_t* g_board;

std::vector<card_t*> g_all_cards;

std::vector<hand_t*> g_all_hands;

std::vector<range_t*> g_open_ranges, g_facing_raise_ranges, g_facing_3bet_ranges, g_facing_4bet_ranges;

// Handling the lookup of dll$symbols
DLL_IMPLEMENTS double __stdcall ProcessQuery(const char* pquery)
{
	if (nullptr == pquery)
	{
		return -1;
	}
	std::string query = std::string(pquery);
	if (query == "dll$test")
	{
		try
		{
			update_board(g_board);
		}
		catch (std::exception & e)
		{
			LOG_F(FATAL, e.what());
		}
	}
	return 0;
}


// TODO: msg larisa dupa ce termin
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) 
{
	FILE* conout = nullptr;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			int argc = 1;
			char argv_0[MAX_PATH];
			DWORD dError = GetModuleFileNameA(NULL, argv_0, MAX_PATH);
			if (dError == 0)
			{
				return GetLastError();
			}
			char* argv[2];
			argv[0] = argv_0;
			argv[1] = nullptr;
			loguru::init(argc, argv);

			g_board = create_board();
			g_all_cards = create_all_cards();
			g_all_hands = create_all_hands();

			g_open_ranges = create_open_ranges();
			g_facing_raise_ranges = create_facing_raise_ranges();
			g_facing_3bet_ranges = create_facing_3bet_ranges();
			g_facing_4bet_ranges = create_facing_4bet_ranges();

			InitializeOpenHoldemFunctionInterface();
			dll_process_attach(&conout);
		} break;

		case DLL_PROCESS_DETACH:
		{
			destroy_board(&g_board);
			dll_process_detach(conout);
		} break;

		default:
		{
		}
	}
	return TRUE;
} 
