#include "position.h"
#include <cassert>


position_t& operator++(position_t& position)
{
	assert(position != INVALID_POSITION && position != POSITION_COUNT);
	if (position == BB)
	{
		position = UTG;
	}
	else
	{
		position = (position_t)((int)position - 1);
	}
	return position;
}


position_t& operator--(position_t& position)
{
	assert(position != INVALID_POSITION && position != POSITION_COUNT);
	if (position == UTG)
	{
		position = BB;
	}
	else
	{
		position = (position_t)((int)position + 1);
	}
	return position;
}


position_t operator+(const position_t& position, int to_add)
{
	int position_int = (int)position;
	position_int += to_add;
	assert(position_int <= POSITION_COUNT && position_int >= UTG);
	return (position_t)position_int;
}
