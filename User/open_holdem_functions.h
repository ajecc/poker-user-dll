#pragma once

#include <string>


const char*
get_symbol(const char* symbol);


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
