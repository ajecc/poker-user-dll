
#include "open_holdem_functions.h"
#include "atlstr.h"
#include "debug.h"
#include <cstddef>
#include <map>
#include <string>

extern std::map<std::string, std::string> g_symbols;


const char*
get_symbol_value(const char* symbol)
{
	return g_symbols[std::string(symbol)].c_str();
}


std::string
scrape_table_map_region(const std::string& region)
{
	DLOG(INFO, "region = %s", region.c_str());
	const char* response = get_symbol_value(region.c_str());
	if (response == nullptr)
	{
		DLOG(INFO, "response = NULL");
		return std::string("");
	}
	DLOG(INFO, "response = %s", response);
	return std::string(response);
}


float
scrape_table_map_region_numeric(const std::string& region)
{
	std::string response = scrape_table_map_region(region);
	std::string clean_response = "";
	for (const auto& c : response)
	{
		if ((c >= '0' && c <= '9') || c == '.')
		{
			clean_response += c;
		}
	}
	if (clean_response.empty())
	{
		clean_response.push_back('0');
	}
	return std::stof(clean_response);
}
