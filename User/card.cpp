#include <string>
#include <algorithm>
#include <cassert>
#include "card.h"
#include "poker_exception.h"
#include "util.h"


extern std::vector<card_t*> g_all_cards;


color_t& operator++(color_t& color)
{
#ifdef _DEBUG
	if (color == COLOR_COUNT)
	{
		throw poker_exception_t("color_t++: overflow");
	}
#endif
	color = (color_t)((int)color + 1);
	return color;
}


rank_t& operator++(rank_t& rank)
{
#ifdef _DEBUG
	if (rank == RANK_COUNT)
	{
		throw poker_exception_t("rank_t++: overflow");
	}
#endif
	rank = (rank_t)((int)rank + 1);
	return rank;
}


bool card_t::operator<(const card_t& other) const
{
	if (rank == other.rank)
	{
		return color < other.color;
	}
	return rank < other.rank;
}


bool card_t::operator>(const card_t& other) const
{
	if (rank == other.rank)
	{
		return color > other.color;
	}
	return rank > other.rank;
}


bool card_t::operator==(const card_t& other) const
{
	return color == other.color && rank == other.rank;
}


bool card_t::operator!=(const card_t& other) const
{
	return !(*this == other);
}


card_t& operator++(card_t& card)
{
	assert(card != *get_card(_A, S));
	if (card.color != S)
	{
		++card.color;
	}
	else
	{
		card.color = H;
		++card.rank;
	}
	return card;
}


std::vector<card_t*> create_all_cards()
{
	std::vector<card_t*> all_cards;
	for (rank_t rank = _2; rank <= _A; ++rank)
	{
		for (color_t color = H; color <= S; color = (color_t)((int)color + 1))
		{
			card_t* card = new card_t;
			*card = { color, rank };
			all_cards.emplace_back(card);
		}
	}
	return all_cards;
}


int get_card_index(const rank_t& rank, const color_t& color)
{
	int rank_to_int = (int)rank - 2;
	int color_to_int = (int)color;
	return rank_to_int * 4 + color_to_int;
}


int get_card_index(card_t* card)
{
	return get_card_index(card->rank, card->color);
}


card_t* get_card(rank_t rank, color_t color)
{
	return g_all_cards[get_card_index(rank, color)];
}


bool is_in_vector(card_t* card, const std::vector<card_t*>& vec)
{
	for (card_t* other : vec)
	{
		if (*card == *other)
		{
			return true;
		}
	}
	return false;
}


card_t* get_card(const std::string& str)
{
	if (str.size() != 2)
	{
		throw poker_exception_t("get_card_from_string: invalid string (" + str + ")");
	}
	card_t card;
	char card_rank = str[0];
	char card_color = str[1];

	if (card_rank >= '2' && card_rank <= '9')
	{
		card.rank = (rank_t)(card_rank - '2' + 2);
	}
	else
	{
		switch (card_rank)
		{
		case 'T':
			card.rank = _T;
			break;
		case 'J':
			card.rank = _J;
			break;
		case 'Q':
			card.rank = _Q;
			break;
		case 'K':
			card.rank = _K;
			break;
		case 'A':
			card.rank = _A;
			break;
		default:
			throw poker_exception_t("get_card_from_string: invalid string (" + str + ")");
		}
	}
	switch (card_color)
	{
	case 'h':
		card.color = H;
		break;
	case 'd':
		card.color = D;
		break;
	case 'c':
		card.color = C;
		break;
	case 's':
		card.color = S;
		break;
	default:
		throw poker_exception_t("get_card_from_string: invalid string (" + str + ")");
	}
	return get_card(card.rank, card.color);
}


std::string card_t::to_string()
{
	std::string to_string;
	if (rank >= _2 && rank <= _9)
	{
		to_string = std::to_string(rank);
	}
	else
	{
		switch (rank)
		{
		case _T:
			to_string = "T";
			break;
		case _J:
			to_string = "J";
			break;
		case _Q:
			to_string = "Q";
			break;
		case _K:
			to_string = "K";
			break;
		case _A:
			to_string = "A";
			break;
		default:
			throw poker_exception_t("Invalid card");
		}
	}
	switch (color)
	{
	case H:
		to_string += "h";
		break;
	case D:
		to_string += "d";
		break;
	case C:
		to_string += "c";
		break;
	case S:
		to_string += "s";
		break;
	default:
		throw poker_exception_t("Invalid card");
	}
	return to_string;
}
