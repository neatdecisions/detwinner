#include <gtest/gtest.h>

#include <logic/images/HistogramT.hpp>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_different)
{
	HistogramT<4> hist1;
	hist1.setBinValue(0, 5);
	hist1.setBinValue(1, 4);
	hist1.setBinValue(2, 8);
	hist1.setBinValue(3, 3);

	HistogramT<4> hist2;
	hist2.setBinValue(0, 9);
	hist2.setBinValue(1, 100);
	hist2.setBinValue(2, 0);
	hist2.setBinValue(3, 12);

	EXPECT_FLOAT_EQ(0.977230543567177f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.977230543567177f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_equal)
{
	HistogramT<4> hist1;
	hist1.setBinValue(0, 5);
	hist1.setBinValue(1, 4);
	hist1.setBinValue(2, 8);
	hist1.setBinValue(3, 3);

	HistogramT<4> hist2;
	hist2.setBinValue(0, 5);
	hist2.setBinValue(1, 4);
	hist2.setBinValue(2, 8);
	hist2.setBinValue(3, 3);

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.0f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_same)
{
	HistogramT<4> hist1;
	hist1.setBinValue(0, 5);
	hist1.setBinValue(1, 4);
	hist1.setBinValue(2, 8);
	hist1.setBinValue(3, 3);

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_zero_one)
{
	HistogramT<4> hist1;
	hist1.setBinValue(0, 0);
	hist1.setBinValue(1, 0);
	hist1.setBinValue(2, 0);
	hist1.setBinValue(3, 0);

	HistogramT<4> hist2;
	hist1.setBinValue(0, 1);
	hist1.setBinValue(1, 2);
	hist1.setBinValue(2, 3);
	hist1.setBinValue(3, 4);

	EXPECT_FLOAT_EQ(0.905555555555556f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.905555555555556f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_zero_both)
{
	HistogramT<4> hist1;
	hist1.setBinValue(0, 0);
	hist1.setBinValue(1, 0);
	hist1.setBinValue(2, 0);
	hist1.setBinValue(3, 0);

	HistogramT<4> hist2;
	hist1.setBinValue(0, 0);
	hist1.setBinValue(1, 0);
	hist1.setBinValue(2, 0);
	hist1.setBinValue(3, 0);

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.0f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_zero_uninitialized)
{
	HistogramT<4> hist1;

	HistogramT<4> hist2;
	hist1.setBinValue(0, 0);
	hist1.setBinValue(1, 0);
	hist1.setBinValue(2, 0);
	hist1.setBinValue(3, 0);

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.0f, hist2.compare(hist1));
}


}}}
