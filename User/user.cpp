// NOTE: Required OpenHoldem version: 12.1.6 ?? 

// Needs to be defined here, before #include "user.h"
// to generate proper export- and inport-definitions
#define USER_DLL

#include <conio.h>
#include <Windows.h>
#include <cstdio>
#include <random>
#include "user.h"
#include "open_holdem_functions.h"
#include "board.h"



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
		
	}
	return 0;
}


// TODO: msg larisa dupa ce termin
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) 
{
	FILE* file = nullptr;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			g_board = create_board();
#ifdef _DEBUG
			AllocConsole();
			freopen_s(&file, "CONOUT$", "w", stdout);
			printf_s("The log console is running!\n");
			InitializeOpenHoldemFunctionInterface();
#endif
		} break;

		case DLL_PROCESS_DETACH:
		{
#ifdef _DEBUG
			FreeConsole();
			if (file != nullptr)
			{
				fclose(file);
			}
#endif
		} break;

		default:
		{
		}
	}
	return TRUE;
} 
