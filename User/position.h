#pragma once


enum position_t
{
	INVALID_POSITION = -1,
	BB = 5,
	SB = 4,
	BTN = 3,
	CO = 2,
	HJ = 1,
	UTG = 0,
	POSITION_COUNT = 6
};


position_t& 
operator++(position_t& position);

position_t& 
operator--(position_t& position);

position_t
operator+(const position_t& position, int to_add);
