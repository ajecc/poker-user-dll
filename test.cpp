#include "pch.h"
#include "../User/user.h"

#include "test_card.h"


int main(int argc, char** argv)
{
	create_globals();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}