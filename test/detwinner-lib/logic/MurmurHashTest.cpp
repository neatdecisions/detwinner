#include <gtest/gtest.h>

#include <logic/MurmurHash.hpp>

namespace detwinner::logic {

//------------------------------------------------------------------------------
TEST(MurmurHashTest, basic)
{
	std::string hash;
	EXPECT_FALSE(BuildMurmurHash("data/foobar", hash));
	EXPECT_TRUE(BuildMurmurHash("data/files/file1.txt", hash));
	EXPECT_EQ("8774722d0cfeb65f14fdd1aff0ad3456", hash);
	EXPECT_TRUE(BuildMurmurHash("data/files/empty.foo", hash));
	EXPECT_EQ("cf8ea65009661ad009661ad009661ad0", hash);
	EXPECT_TRUE(BuildMurmurHash("data/files/file2.txt", hash));
	EXPECT_EQ("8774722d0cfeb65f14fdd1aff0ad3456", hash);
	EXPECT_TRUE(BuildMurmurHash("data/images/gm-125x80.gif", hash));
	EXPECT_EQ("85280b5367eaf701cedbc0cb95ec61ba", hash);
	EXPECT_TRUE(BuildMurmurHash("data/images/gm-125x80.png", hash));
	EXPECT_EQ("5bf4e678d500fb1eed246fa6739177c0", hash);
	EXPECT_TRUE(BuildMurmurHash("data/images/gm-125x80t (copy).png", hash));
	EXPECT_EQ("3d1fd672b06c0bdd573df16af5bd1d1e", hash);
	EXPECT_TRUE(BuildMurmurHash("data/images/gm-125x80t.png", hash));
	EXPECT_EQ("33867bec3928ba51adc33ee93c7e3f66", hash);
	EXPECT_TRUE(BuildMurmurHash("data/images/gm-654x418t.png", hash));
	EXPECT_EQ("de2bf3f18c732e983b8f563a50a747be", hash);
}

} // namespace detwinner::logic
