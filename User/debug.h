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
	std::string log_file_name = "user_dll_debug_";
	log_file_name += std::to_string(GetCurrentProcessId()) + ".log";
	bool bError = 
		loguru::add_file(log_file_name.c_str(), loguru::Append, loguru::Verbosity_MAX);
	if (bError == false)
	{
		exit(-1);
	}
	DLOG(INFO, "Console is open");
}


inline void 
uninit_log(FILE* conout)
{
	//FreeConsole();
	//if (conout != nullptr)
	//{
	//	fclose(conout);
	//}
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
