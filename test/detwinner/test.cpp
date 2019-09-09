#include <gtest/gtest.h>
#include <glibmm/init.h>


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	Glib::init();
	return RUN_ALL_TESTS();
}
