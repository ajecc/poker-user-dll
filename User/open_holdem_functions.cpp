
#include "open_holdem_functions.h"
#include "atlstr.h"
#include <cstddef>

// Function signatures and pointers

typedef double(*t_GetSymbol)(const char* name_of_single_symbol__not_expression);
typedef void*(*t_GetPrw1326)();
typedef char*(*t_GetHandnumber)();
typedef char*(*t_GetPlayerName)(int chair);
typedef char*(*t_GetTableTitle)();
typedef void(*t_ParseHandList)(const char* name_of_list, const char* list_body);
typedef char*(*t_ScrapeTableMapRegion)(const char* p_region, int& p_returned_lengh);  // TODO: check if the first argument really is a const char
typedef void(*t_SendChatMessage)(char *message);
typedef void(*t_WriteLog)(char* format, va_list args);

t_GetSymbol p_GetSymbol = nullptr;
t_GetPrw1326 p_GetPrw1326 = nullptr;
t_GetHandnumber p_GetHandnumber = nullptr;
t_GetPlayerName p_GetPlayerName = nullptr;
t_GetTableTitle p_GetTableTitle = nullptr;
t_ParseHandList p_ParseHandList = nullptr;
t_ScrapeTableMapRegion p_ScrapeTableMapRegion = nullptr;
t_SendChatMessage p_SendChatMessage = nullptr;
t_WriteLog p_WriteLog = nullptr;



// OpenHoldem functions, to be used by the DLL

void ErrorPointerNotInitialized(const char* function_name);


double __stdcall GetSymbol(const char* name_of_single_symbol__not_expression)
{
	if (p_GetSymbol == nullptr) {
		ErrorPointerNotInitialized("GetSymbol");
		return 0.0;
	}
	return p_GetSymbol(name_of_single_symbol__not_expression);
}


void* __stdcall GetPrw1326()
{
	if (p_GetPrw1326 == nullptr)
	{
		ErrorPointerNotInitialized("GetPrw1326");
		return nullptr;
	}
	return p_GetPrw1326();
}


char* __stdcall GetHandnumber() {
	if (p_GetHandnumber == nullptr)
	{
		ErrorPointerNotInitialized("GetHandnumber");
		return NULL;
	}
	return p_GetHandnumber();
}


char* __stdcall GetPlayerName(int chair) 
{
	if (p_GetPlayerName == nullptr)
	{
		ErrorPointerNotInitialized("GetPlayerName");
		return NULL;
	}
	return p_GetPlayerName(chair);
}


char* __stdcall GetTableTitle()
{
	if (p_GetTableTitle == nullptr)
	{
		ErrorPointerNotInitialized("GetTableTitle");
		return NULL;
	}
	return p_GetTableTitle();
}


void __stdcall ParseHandList(const char* name_of_list, const char* list_body)
{
	if (p_ParseHandList == nullptr)
	{
		ErrorPointerNotInitialized("ParseHandList");
		return;
	}
	p_ParseHandList(name_of_list, list_body);
}


char* __stdcall ScrapeTableMapRegion(const char* p_region, int& p_returned_lengh) {

	if (p_ScrapeTableMapRegion == nullptr)
	{
		ErrorPointerNotInitialized("ScrapeTableMapRegion");
		return NULL;
	}
	return p_ScrapeTableMapRegion(p_region, p_returned_lengh);
}


void __stdcall SendChatMessage(char *message)
{
	if (p_SendChatMessage == nullptr)
	{
		ErrorPointerNotInitialized("SendChatMessage");
		return;
	}
	p_SendChatMessage(message);
}


void WriteLog(char* format, ...)
{
	if (p_WriteLog == nullptr)
	{
		ErrorPointerNotInitialized("WriteLog");
		return;
	}
	va_list args;
	va_start(args, format);
	p_WriteLog(format, args);
	va_end(args);
}


// Initialization of the interface
FARPROC WINAPI LookupOpenHoldemFunction(const char* function_name)
{
	HMODULE openholdem_main_module = GetModuleHandle(NULL);
	if (openholdem_main_module == nullptr)
	{
		return nullptr;
	}
	FARPROC function_address = GetProcAddress(openholdem_main_module, function_name);
	if (function_address == nullptr)
	{
		CString error_message;
		error_message.Format(
				"Can not find %s in Openholdem.exe.\n",
				function_name);
				MessageBox(0,
				error_message,
				"DLL Error",
				MB_ICONERROR
				);
	}
	return function_address;
}


void InitializeOpenHoldemFunctionInterface()
{
	p_GetSymbol = (t_GetSymbol)LookupOpenHoldemFunction("GetSymbol");
	p_GetPrw1326 = (t_GetPrw1326)LookupOpenHoldemFunction("GetPrw1326");
	p_GetHandnumber = (t_GetHandnumber)LookupOpenHoldemFunction("GetHandnumber");
	p_GetPlayerName = (t_GetPlayerName)LookupOpenHoldemFunction("GetPlayerName");
	p_GetTableTitle = (t_GetTableTitle) LookupOpenHoldemFunction("GetTableTitle");
	p_ParseHandList = (t_ParseHandList)LookupOpenHoldemFunction("ParseHandList");
	p_ScrapeTableMapRegion = (t_ScrapeTableMapRegion)LookupOpenHoldemFunction("ScrapeTableMapRegion");
	p_SendChatMessage = (t_SendChatMessage)LookupOpenHoldemFunction("SendChatMessage");
	p_WriteLog = (t_WriteLog)LookupOpenHoldemFunction("WriteLog");
}


void ErrorPointerNotInitialized(const char* function_name)
{
	CString error_message;
	error_message.Format(
		"OpenHoldem interface not yet initialized.\nCan't use function %s.\n",
		function_name
	);
	MessageBox(
		0,
		error_message,
		"DLL Error",
		MB_ICONERROR
	);
}

std::string scrape_table_map_region(const std::string& p_region)
{
	int dummy;
	char* response = ScrapeTableMapRegion(p_region.c_str(), dummy);
	return std::string(response);
}


double scrape_table_map_region_numeric(const std::string& p_region)
{
	std::string response = scrape_table_map_region(p_region);
	std::string clean_response = "";
	for (const auto& c : response)
	{
		if (c >= '0' || c <= '9' || c == '.')
		{
			clean_response += c;
		}
	}
	if (clean_response.empty())
	{
		clean_response.push_back('0');
	}
	return std::stod(clean_response);
}
