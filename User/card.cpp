#include <string>
#include "card.h"
#include "poker_exception.h"


card_t* get_card_from_string(const std::string& str)
{
	if (str.size() != 2)
	{
		return nullptr;
	}
	card_t* card = new card_t;
	char card_rank = str[0];
	char card_color = str[1];

	if (card_rank >= '2' && card_rank <= '9')
	{
		card->rank = (rank_t)(card_rank - '2' + 2);
	}
	else
	{
		switch (card_rank)
		{
		case 'T':
			card->rank = _T;
			break;
		case 'J':
			card->rank = _J;
			break;
		case 'Q':
			card->rank = _Q;
			break;
		case 'K':
			card->rank = _K;
			break;
		case 'A':
			card->rank = _A;
			break;
		default:
			goto cleanup;
		}
	}
	switch (card_color)
	{
	case 'h':
		card->color = H;
		break;
	case 'd':
		card->color = D;
		break;
	case 'c':
		card->color = C;
		break;
	case 's':
		card->color = S;
		break;
	default:
		goto cleanup;
	}

	return card;

cleanup:
	delete card;
	return nullptr;
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
