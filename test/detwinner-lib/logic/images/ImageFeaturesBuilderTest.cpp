#include <gtest/gtest.h>

#include <logic/images/ImageFeaturesBuilder.hpp>

#include "../mocks/MockImageFinderCallback.hpp"
#include "ImageFeaturesTestFactory.hpp"

using ::testing::_;
using ::testing::Return;

namespace detwinner::logic::images {

//==============================================================================
// ImageFeaturesBuilderTest
//==============================================================================
struct ImageFeaturesBuilderTest : public ::testing::Test
{
	ImageFeaturesBuilderTest()
			: m_feat1(ImageFeaturesTestFactory::CreateFeatures_1()), m_feat2(ImageFeaturesTestFactory::CreateFeatures_2())
	{
	}

	ImageFeatures m_feat1;
	ImageFeatures m_feat2;
};

//==============================================================================
// ImageFeaturesBuilderTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(ImageFeaturesBuilderTest, basic_no_callback)
{
	const std::vector<std::string> fileNames = {"data/images/gm-125x80.png", "data/images/gm-125x80t.png"};

	ImageFeaturesBuilder builder(fileNames, nullptr);
	const std::vector<ImageFeatures> imageFeatures = builder.execute();
	EXPECT_EQ(2UL, imageFeatures.size());
	EXPECT_FLOAT_EQ(0.0f, imageFeatures[0].compare(m_feat1, false));
	EXPECT_FLOAT_EQ(0.0f, imageFeatures[1].compare(m_feat2, false));
}

//------------------------------------------------------------------------------
TEST_F(ImageFeaturesBuilderTest, basic_callback)
{
	const std::vector<std::string> fileNames = {"data/images/gm-125x80.png", "data/images/gm-125x80t.png"};

	callbacks::mocks::MockImageFinderCallback::Ptr pMockedCallback = callbacks::mocks::MockImageFinderCallback::Create();
	ON_CALL(*pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(false));
	EXPECT_CALL(*pMockedCallback, imgComparingProgress(_, _)).Times(0);
	EXPECT_CALL(*pMockedCallback, imgIndexingProgress(0, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgIndexingProgress(1, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgIndexingProgress(2, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgOrganizingProgress(_, _)).Times(0);
	EXPECT_CALL(*pMockedCallback, similarImagesFound(_, _, 0)).Times(0);

	ImageFeaturesBuilder builder(fileNames, pMockedCallback);
	const std::vector<ImageFeatures> imageFeatures = builder.execute();
	ASSERT_EQ(2UL, imageFeatures.size());
	EXPECT_FLOAT_EQ(0.0f, imageFeatures[0].compare(m_feat1, false));
	EXPECT_FLOAT_EQ(0.0f, imageFeatures[1].compare(m_feat2, false));
}

//------------------------------------------------------------------------------
TEST_F(ImageFeaturesBuilderTest, basic_callback_interrupted)
{
	const std::vector<std::string> fileNames = {"data/images/gm-125x80.png", "data/images/gm-125x80t.png"};

	callbacks::mocks::MockImageFinderCallback::Ptr pMockedCallback = callbacks::mocks::MockImageFinderCallback::Create();
	ON_CALL(*pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(true));
	EXPECT_CALL(*pMockedCallback, imgComparingProgress(_, _)).Times(0);
	EXPECT_CALL(*pMockedCallback, imgIndexingProgress(0, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgIndexingProgress(1, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgOrganizingProgress(_, _)).Times(0);
	EXPECT_CALL(*pMockedCallback, similarImagesFound(_, _, 0)).Times(0);

	ImageFeaturesBuilder builder(fileNames, pMockedCallback);
	const std::vector<ImageFeatures> imageFeatures = builder.execute();
	ASSERT_EQ(1UL, imageFeatures.size());
	EXPECT_FLOAT_EQ(0.0f, imageFeatures[0].compare(m_feat1, false));
}

} // namespace detwinner::logic::images
