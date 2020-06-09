#pragma once
#include <string>
#include <vector>

#define NUMBER_OF_CARDS 52

enum color_t
{
	INVALID_COLOR = -1,
	H = 0,
	D = 1,
	C = 2,
	S = 3,
	COLOR_COUNT = 4
};

color_t& operator++(color_t& color);

enum rank_t
{
	INVALID_RANK,
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

rank_t&
operator++(rank_t& rank);


struct card_t
{
	color_t color = INVALID_COLOR;
	rank_t rank = INVALID_RANK;

	bool 
	operator<(const card_t& other) const;

	bool
	operator>(const card_t& other) const;

	bool
	operator==(const card_t& other) const;

	bool
	operator!=(const card_t& other) const;

	std::string
	to_string() const;
};

card_t&
operator++(card_t& card);

inline bool
card_ptr_cmp(const card_t* lhs, const card_t* rhs)
{
	return *lhs < *rhs;
}



/*
Fetches the index of a card, as it is found in the vector returned by
create_all_cards.

Parameters: IN rank_t rank -- the rank of the card
			IN color_t color -- the color of the card

Returns: int -- the index of the card
*/
int
get_card_index(const rank_t& rank, const color_t& color);


/*
Fetches the index of a card, as it is found in the vector returned by
create_all_cards.

Parameters: IN card_t* card -- pointer to the card to find the index of

Returns: int -- the index of the card
*/
int
get_card_index(const card_t* card);


/*
Gets the card based on rank and color. This is fetched from the vector
return by create_all_cards.  

Parameters: IN rank_t rank -- the rank of the card
			IN color_t color -- the color of the card

Returns: card_t* -- the pointer to the card. This SHOULD NOT be freed.
*/
const card_t*
get_card(rank_t rank, color_t color);


/*
Gets the card, but from string form. Works in the same way as get_card.
The string has for {RANK}{COLOR}. Eg: Ah, 6s.

Returns: card_t* -- the pointer to the card, that shouldn't be freed.
*/
const card_t*
get_card(const std::string& str);


/*
Checks if a card is in a vector, based on its value, not address.

Parameters: IN card_t* card -- the card
			IN std::vector<card_t*> vec -- the vector that contains various cards

Returns: bool -- true if is found in the vector and false otherwise.
*/
bool
is_in_vector(const card_t* card, const std::vector<const card_t*>& vec);


/*
Creates all possible combinations of cards. The vector returned by this is a global one.
Cards can be fetched using get_card.
This functions should only be called at attachment.

Parameters: none

Return: std::vector<card_t*> -- the vector that contains all the cards.
								Should not be used explicitly (use get_card instead)
*/
std::vector<const card_t*>
create_all_cards();
