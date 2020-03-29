#include "pch.h"
#include "../User/user.h"

#include "test_card.h"


int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	create_globals();
	return RUN_ALL_TESTS();
}
