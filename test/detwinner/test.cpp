#include <glibmm/init.h>
#include <gtest/gtest.h>

int
main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	Glib::init();
	return RUN_ALL_TESTS();
}
