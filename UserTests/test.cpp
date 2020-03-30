#include "pch.h"
#include "../User/user.h"
#include <iostream>

#include "test_card.h"
#include "test_hand_board_result.h"
#include "test_hand.h"



int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	create_globals();
	return RUN_ALL_TESTS();
}
