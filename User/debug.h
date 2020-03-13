#pragma once
#include <conio.h>
#include <Windows.h>
#include <cstdio>
#include "loguru.h"


#ifdef _DEBUG
void dll_process_attach(FILE** conout)
{

	AllocConsole();
	freopen_s(conout, "CONOUT$", "w", stdout);
	bool bError = 
		loguru::add_file("user_dll_debug.log", loguru::Truncate, loguru::Verbosity_MAX);
	if (bError == false)
	{
		exit(-1);
	}
	bError = loguru::add_file("CONOUT$", loguru::Truncate, loguru::Verbosity_MAX);
	if (bError == false)
	{
		exit(-1);
	}
}


void dll_process_detach(FILE* conout)
{
	FreeConsole();
	if (conout != nullptr)
	{
		fclose(conout);
	}
}
#define LOG_DEBUG LOG_F
#else
void dll_process_attach(FILE** conout)
{
	UNREFERENCED_PARAMETER(conout);
}


void dll_process_detach(FILE* conout)
{
	UNREFERENCED_PARAMETER(conout);
}

#define LOG_DEBUG
#endif

