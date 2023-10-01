#include <gtest/gtest.h>

#include <algorithm>
#include <thread>

#include <logic/images/SimilarityCacheBuilder.hpp>

#include "../mocks/MockImageFinderCallback.hpp"
#include "ImageFeaturesTestFactory.hpp"

using ::testing::_;
using ::testing::Return;

namespace detwinner::logic::images {

//==============================================================================
// SimilarityCacheBuilderTest
//==============================================================================
struct SimilarityCacheBuilderTest : public ::testing::Test
{
	const unsigned int m_threadCount = std::max(4U, std::thread::hardware_concurrency());
};

//==============================================================================
// SimilarityCacheBuilderTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(SimilarityCacheBuilderTest, parallel_indexes_normal)
{
	const SimilarityCacheBuilder::ParallelIndexes_t indexes = SimilarityCacheBuilder::CalculateParallelIndexes(103, 100);
	EXPECT_EQ(m_threadCount, indexes.size());
	std::size_t assembledCount = 0;
	for (const auto & val : indexes)
	{
		EXPECT_LE(val.first, val.second);
		assembledCount += (val.second - val.first);
	}
	EXPECT_EQ(103ULL, assembledCount);
}

//------------------------------------------------------------------------------
TEST_F(SimilarityCacheBuilderTest, parallel_indexes_small)
{
	const SimilarityCacheBuilder::ParallelIndexes_t indexes = SimilarityCacheBuilder::CalculateParallelIndexes(5, 100);
	EXPECT_EQ(0ULL, indexes.size());
}

//------------------------------------------------------------------------------
TEST_F(SimilarityCacheBuilderTest, execute_basic_rotations)
{
	const std::vector<ImageFeatures> imageFeatures = {ImageFeaturesTestFactory::CreateFeatures_1(),
	                                                  ImageFeaturesTestFactory::CreateFeatures_2()};
	SimilarityCache cache = SimilarityCacheBuilder(imageFeatures, true, nullptr).execute();
	EXPECT_EQ(30U, cache.get(0, 1));
	EXPECT_EQ(30U, cache.get(1, 0));
	EXPECT_EQ(DISTANCE_T_MAX, cache.get(1, 2));
	EXPECT_EQ(0U, cache.get(1, 1));
}

//------------------------------------------------------------------------------
TEST_F(SimilarityCacheBuilderTest, execute_basic_no_rotations)
{
	const std::vector<ImageFeatures> imageFeatures = {ImageFeaturesTestFactory::CreateFeatures_1(),
	                                                  ImageFeaturesTestFactory::CreateFeatures_2()};

	callbacks::mocks::MockImageFinderCallback::Ptr pMockedCallback = callbacks::mocks::MockImageFinderCallback::Create();
	ON_CALL(*pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(false));
	EXPECT_CALL(*pMockedCallback, imgComparingProgress(0, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgComparingProgress(1, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgComparingProgress(2, 2)).Times(1);
	EXPECT_CALL(*pMockedCallback, imgIndexingProgress(_, _)).Times(0);
	EXPECT_CALL(*pMockedCallback, imgOrganizingProgress(_, _)).Times(0);
	EXPECT_CALL(*pMockedCallback, similarImagesFound(_, _, 0)).Times(0);

	SimilarityCache cache = SimilarityCacheBuilder(imageFeatures, false, pMockedCallback).execute();
	EXPECT_EQ(39U, cache.get(0, 1));
	EXPECT_EQ(39U, cache.get(1, 0));
	EXPECT_EQ(DISTANCE_T_MAX, cache.get(1, 2));
	EXPECT_EQ(0U, cache.get(1, 1));
}

} // namespace detwinner::logic::images
