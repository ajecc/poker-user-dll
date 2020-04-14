#pragma once
#include <string>
#include <vector>

#define NUMBER_OF_CARDS 52

enum color_t
{
	H,
	D,
	C,
	S,
	COLOR_COUNT
};

color_t& operator++(color_t& color);

enum rank_t
{
	pad_1,
	_A_1,
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

rank_t& operator++(rank_t& rank);


struct card_t
{
	color_t color;
	rank_t rank;

	bool operator<(const card_t& other) const;

	bool operator>(const card_t& other) const;

	bool operator==(const card_t& other) const;

	bool operator!=(const card_t& other) const;

	std::string to_string();
};

card_t& operator++(card_t& card);


std::vector<card_t*> create_all_cards();

int get_card_index(const rank_t& rank, const color_t& color);

int get_card_index(card_t* card);

card_t* get_card(rank_t rank, color_t color);

bool is_in_vector(card_t* card, const std::vector<card_t*>& vec);

card_t* get_card_from_string(const std::string& str);
