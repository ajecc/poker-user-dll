#include "range.h"

void range_t::add(hand_t* hand)
{
	range.insert(hand);
}


void range_t::remove(hand_t* hand)
{
	range.erase(hand);
}


bool range_t::contains(hand_t* hand)
{
	return range.find(hand) != range.end();
}


std::vector<range_t*> create_hero_open_range()
{
	// TODO: add call, raise, fold probabilities

}
