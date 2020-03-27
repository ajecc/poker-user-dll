#pragma once
#include <conio.h>
#include <Windows.h>
#include <cstdio>
#include "loguru.h"


#ifdef _DEBUG
#define DLOG DLOG_F

inline void dll_process_attach(FILE** conout)
{

	AllocConsole();
	freopen_s(conout, "CONOUT$", "w", stdout);
	bool bError = 
		loguru::add_file("user_dll_debug.log", loguru::Append, loguru::Verbosity_MAX);
	if (bError == false)
	{
		exit(-1);
	}
	bError = loguru::add_file("CONOUT$", loguru::Truncate, loguru::Verbosity_MAX);
	if (bError == false)
	{
		exit(-1);
	}
	DLOG(INFO, "Console is open");
}


inline void dll_process_detach(FILE* conout)
{
	FreeConsole();
	if (conout != nullptr)
	{
		fclose(conout);
	}
}
#else
inline void dll_process_attach(FILE** conout)
{
	UNREFERENCED_PARAMETER(conout);
}


inline void dll_process_detach(FILE* conout)
{
	UNREFERENCED_PARAMETER(conout);
}

#define DLOG 
#endif

