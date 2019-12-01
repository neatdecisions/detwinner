#include <gtest/gtest.h>

#include <logic/images/HistogramT.hpp>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_different)
{
	HistogramT<4> hist1;
	hist1.bins = { 5, 4, 8, 3 };

	HistogramT<4> hist2;
	hist2.bins = { 9, 100, 0, 12 };

	EXPECT_FLOAT_EQ(0.977230543567177f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.977230543567177f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_equal)
{
	HistogramT<4> hist1;
	hist1.bins = { 5, 4, 8, 3 };

	HistogramT<4> hist2;
	hist2.bins = { 5, 4, 8, 3 };

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.0f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_same)
{
	HistogramT<4> hist1;
	hist1.bins = { 5, 4, 8, 3 };

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_zero_one)
{
	HistogramT<4> hist1;
	hist1.bins = { 0, 0, 0, 0 };

	HistogramT<4> hist2;
	hist2.bins = { 1, 2, 3, 4 };

	EXPECT_FLOAT_EQ(0.905555555555556f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.905555555555556f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_zero_both)
{
	HistogramT<4> hist1;
	hist1.bins = { 0, 0, 0, 0 };

	HistogramT<4> hist2;
	hist2.bins = { 0, 0, 0, 0 };

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.0f, hist2.compare(hist1));
}


//------------------------------------------------------------------------------
TEST(HistogramTTest, compare_zero_uninitialized)
{
	HistogramT<4> hist1;

	HistogramT<4> hist2;
	hist2 = { 0, 0, 0, 0 };

	EXPECT_FLOAT_EQ(0.0f, hist1.compare(hist2));
	EXPECT_FLOAT_EQ(0.0f, hist2.compare(hist1));
}


}}}
