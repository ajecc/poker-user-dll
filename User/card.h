#pragma once
#include <string>
#include <vector>


enum color_t
{
	H,
	D,
	C,
	S,
	COLOR_COUNT
};



enum rank_t
{
	pad_1,
	pad_2,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	_9,
	_T,
	_J,
	_Q,
	_K,
	_A,
	RANK_COUNT
};



struct card_t
{
	color_t color;
	rank_t rank;

	bool operator<(const card_t& other) const;

	std::string to_string();
};


std::vector<card_t> create_all_cards();

card_t* get_card_from_string(const std::string& str);
