#include <gtest/gtest.h>

#include <logic/images/SimilarImageFinder.hpp>
#include <logic/Initializer.hpp>

#include "../mocks/MockImageFinderCallback.hpp"
#include "../TestingHelpers.hpp"

using ::testing::AtLeast;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::_;


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// SimilarImageFinderTest
//==============================================================================
struct SimilarImageFinderTest : public ::testing::Test
{
	void SetUp() override
	{
		Initialize();
		m_pMockedCallback = callbacks::mocks::MockImageFinderCallback::Create();
		ON_CALL(*m_pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(false));
	}

	SimilarImageFinder m_finder;
	callbacks::mocks::MockImageFinderCallback::Ptr m_pMockedCallback;
};



//==============================================================================
// SimilarImageFinderTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(SimilarImageFinderTest, empty_input)
{
	const std::vector<std::string> fileNames = {};

	EXPECT_CALL(*m_pMockedCallback, imgIndexingProgress(0, 0)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, imgComparingProgress(_, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, imgOrganizingProgress(_, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, similarImagesFound(_, _, _)).Times(0);
	const DuplicateImageResult result = m_finder.find(fileNames, 50, true, m_pMockedCallback);
	EXPECT_EQ(0UL, result.getImageGroupCount());
}


//------------------------------------------------------------------------------
TEST_F(SimilarImageFinderTest, empty_basic)
{
	const std::vector<std::string> fileNames = {
		"data/images/gm-125x80.gif",
		"data/images/gm-125x80.png",
		"data/images/gm-125x80t (copy).png",
		"data/images/gm-125x80t.png",
		"data/images/gm-654x418t.png"
	};

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, imgIndexingProgress(0, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgIndexingProgress(1, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgIndexingProgress(2, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgIndexingProgress(3, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgIndexingProgress(4, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgIndexingProgress(5, 5)).Times(1);
	}

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, imgComparingProgress(0, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgComparingProgress(1, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgComparingProgress(2, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgComparingProgress(3, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgComparingProgress(4, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgComparingProgress(5, 5)).Times(1);
	}

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, imgOrganizingProgress(0, 5)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, imgOrganizingProgress(::testing::Lt(5), 5)).Times(AtLeast(1));
		EXPECT_CALL(*m_pMockedCallback, imgOrganizingProgress(5, 5)).Times(1);
	}

	const DuplicateImageResult result = m_finder.find(fileNames, 80, true, m_pMockedCallback);
	ASSERT_EQ(2UL, result.getImageGroupCount());

	for (std::size_t i = 0; i < result.getImageGroupCount(); ++i)
	{
		const ImageGroup & ig = result.getImageGroup(i);
		ASSERT_EQ(2UL, ig.getImageCount());
		const ImageInfo & ii1 = ig.getImageInfo(0);
		const ImageInfo & ii2 = ig.getImageInfo(1);
		if (FilePathIncludesFileName(ii1.fileName, "data/images/gm-125x80.gif"))
		{
			EXPECT_EQ(2726ULL, ii1.fileSize);
			EXPECT_EQ(125UL, ii1.width);
			EXPECT_EQ(80UL, ii1.height);
			EXPECT_TRUE(FilePathIncludesFileName(ii2.fileName, "data/images/gm-125x80.png"));
			EXPECT_EQ(2662ULL, ii2.fileSize);
			EXPECT_EQ(125UL, ii2.width);
			EXPECT_EQ(80UL, ii2.height);
		} else
		if (FilePathIncludesFileName(ii1.fileName, "data/images/gm-125x80.png"))
		{
			EXPECT_EQ(2662ULL, ii1.fileSize);
			EXPECT_EQ(125UL, ii1.width);
			EXPECT_EQ(80UL, ii1.height);
			EXPECT_TRUE(FilePathIncludesFileName(ii2.fileName, "data/images/gm-125x80.gif"));
			EXPECT_EQ(2726ULL, ii2.fileSize);
			EXPECT_EQ(125UL, ii2.width);
			EXPECT_EQ(80UL, ii2.height);
		} else
		if (FilePathIncludesFileName(ii1.fileName, "data/images/gm-125x80t.png"))
		{
			EXPECT_EQ(11578ULL, ii1.fileSize);
			EXPECT_EQ(126UL, ii1.width);
			EXPECT_EQ(80UL, ii1.height);
			EXPECT_TRUE(FilePathIncludesFileName(ii2.fileName, "data/images/gm-125x80t (copy).png"));
			EXPECT_EQ(46018ULL, ii2.fileSize);
			EXPECT_EQ(160UL, ii2.width);
			EXPECT_EQ(252UL, ii2.height);
		} else
		if (FilePathIncludesFileName(ii1.fileName, "data/images/gm-125x80t (copy).png"))
		{
			EXPECT_EQ(46018ULL, ii1.fileSize);
			EXPECT_EQ(160UL, ii1.width);
			EXPECT_EQ(252UL, ii1.height);
			EXPECT_TRUE(FilePathIncludesFileName(ii2.fileName, "data/images/gm-125x80t.png"));
			EXPECT_EQ(11578ULL, ii2.fileSize);
			EXPECT_EQ(126UL, ii2.width);
			EXPECT_EQ(80UL, ii2.height);
		} else
		{
			FAIL();
		}
	}
}


//------------------------------------------------------------------------------
TEST_F(SimilarImageFinderTest, interruption)
{
	const std::vector<std::string> fileNames = {
		"data/images/gm-125x80.gif",
		"data/images/gm-125x80.png",
		"data/images/gm-125x80t (copy).png",
		"data/images/gm-125x80t.png",
		"data/images/gm-654x418t.png"
	};
	auto pMockedCallback = callbacks::mocks::MockImageFinderCallback::Create();
	ON_CALL(*pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(true));
	EXPECT_EQ(0UL, m_finder.find(fileNames, 80, true, pMockedCallback).getImageGroupCount());
}


//------------------------------------------------------------------------------
TEST_F(SimilarImageFinderTest, null_callback)
{
	const std::vector<std::string> fileNames = {
		"data/images/gm-125x80.gif",
		"data/images/gm-125x80.png",
		"data/images/gm-125x80t (copy).png",
		"data/images/gm-125x80t.png",
		"data/images/gm-654x418t.png"
	};
	EXPECT_EQ(2UL, m_finder.find(fileNames, 80, true, nullptr).getImageGroupCount());
}


}}}
