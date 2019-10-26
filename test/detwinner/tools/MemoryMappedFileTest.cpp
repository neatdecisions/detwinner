#include <gtest/gtest.h>

#include <tools/MemoryMappedFile.hpp>


namespace detwinner {
namespace tools {


//------------------------------------------------------------------------------
TEST(MemoryMappedFileTest, empty_path)
{
	MemoryMappedFile f("");
	EXPECT_FALSE(f.valid());
	EXPECT_EQ(0UL, f.size());
	EXPECT_EQ(nullptr, f.buffer());
}


//------------------------------------------------------------------------------
TEST(MemoryMappedFileTest, settings_ini)
{
	MemoryMappedFile f("data/settings/settings.ini");
	EXPECT_TRUE(f.valid());
	EXPECT_EQ(551UL, f.size());
	EXPECT_EQ('[', f.buffer()[0]);
	EXPECT_EQ('G', f.buffer()[1]);
	EXPECT_EQ('l', f.buffer()[2]);
	EXPECT_EQ('o', f.buffer()[3]);
	EXPECT_EQ('b', f.buffer()[4]);
	EXPECT_EQ('a', f.buffer()[5]);
	EXPECT_EQ('l', f.buffer()[6]);
}


}}
