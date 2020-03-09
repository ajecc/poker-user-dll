#include <string>
#include "card.h"


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
