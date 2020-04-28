#pragma once

#include "user.h"
#include <string>


/*
OpenHoldem functions.
*/
double __stdcall
GetSymbol(const char* name_of_single_symbol__not_expression);

void* __stdcall
GetPrw1326();

char* __stdcall 
GetHandnumber();

char* __stdcall 
GetPlayerName(int chair);

char* __stdcall
GetTableTitle();

void  __stdcall 
ParseHandList(const char* name_of_list, const char* list_body);

char* __stdcall
ScrapeTableMapRegion(const char* p_region, int& p_returned_lengh);

void  __stdcall 
SendChatMessage(char *message);

void
WriteLog(char* format, ...);

void
InitializeOpenHoldemFunctionInterface();


/*
Wrapper for ScrapeTableMapRegion.
Uses std::strings instead of char*.

Parameters: IN string region -- the region (as defined by OpenHoldem)

Returns: float -- the corresponding string at that region
*/
std::string
scrape_table_map_region(const std::string& region);


/*
Scrapes the region and returns the number that corresponds to that region.
In case nothing is found at that region, 0 is returned.

Parameters: IN string region -- the region (as defined by OpenHoldem)

Returns: float -- the corresponding number at that region
*/
float
scrape_table_map_region_numeric(const std::string& region);
