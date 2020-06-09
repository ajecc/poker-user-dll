#pragma once
#include <conio.h>
#include "clean_windows.h"
#include <cstdio>
#include "loguru.h"


#if 1
//#ifdef _DEBUG
#define DLOG DLOG_F

inline void 
init_log(FILE** conout)
{
	int argc = 1;
	char argv_0[MAX_PATH];
	DWORD dError = GetModuleFileNameA(NULL, argv_0, MAX_PATH);
	if (dError == 0)
	{
		exit(GetLastError());
	}
	char* argv[2];
	argv[0] = argv_0;
	argv[1] = nullptr;
	loguru::init(argc, argv);

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


inline void 
uninit_log(FILE* conout)
{
	FreeConsole();
	if (conout != nullptr)
	{
		fclose(conout);
	}
}

#else

inline void
dll_process_attach(FILE** conout)
{
	UNREFERENCED_PARAMETER(conout);
}


inline void
dll_process_detach(FILE* conout)
{
	UNREFERENCED_PARAMETER(conout);
}

#define DLOG(verbosity, arg) (void)verbosity;void(arg); 

#endif
