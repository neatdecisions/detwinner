#include <gtest/gtest.h>

#include <logic/images/ImageFeatures.hpp>

#include "ImageFeaturesTestFactory.hpp"


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// ImageFeaturesTest
//==============================================================================
struct ImageFeaturesTest : public ::testing::Test
{
	ImageFeaturesTest()	: m_feat1(ImageFeaturesTestFactory::CreateFeatures_1()),
	                      m_feat2(ImageFeaturesTestFactory::CreateFeatures_2())
	{}

	ImageFeatures m_feat1;
	ImageFeatures m_feat2;
};



//==============================================================================
// ImageFeaturesTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(ImageFeaturesTest, no_rotations)
{
	EXPECT_FLOAT_EQ(0.39387715f, m_feat1.compare(m_feat2, false));
	EXPECT_FLOAT_EQ(0.39387715f, m_feat2.compare(m_feat1, false));
}


//------------------------------------------------------------------------------
TEST_F(ImageFeaturesTest, rotations)
{
	EXPECT_FLOAT_EQ(0.30430472f, m_feat1.compare(m_feat2, true));
	EXPECT_FLOAT_EQ(0.30430472f, m_feat2.compare(m_feat1, true));
}


//------------------------------------------------------------------------------
TEST_F(ImageFeaturesTest, same_1)
{
	EXPECT_FLOAT_EQ(0.0f, m_feat1.compare(m_feat1, false));
	EXPECT_FLOAT_EQ(0.0f, m_feat1.compare(m_feat1, true));
}


//------------------------------------------------------------------------------
TEST_F(ImageFeaturesTest, same_2)
{
	EXPECT_FLOAT_EQ(0.0f, m_feat2.compare(m_feat2, false));
	EXPECT_FLOAT_EQ(0.0f, m_feat2.compare(m_feat2, true));
}


}}}
