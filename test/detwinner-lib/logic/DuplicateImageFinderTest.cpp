#include <gtest/gtest.h>

#include <logic/DuplicateImageFinder.hpp>
#include <logic/Initializer.hpp>

#include "mocks/MockSearchProcessCallback.hpp"
#include "TestingHelpers.hpp"

using ::testing::AtLeast;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::_;


namespace detwinner {
namespace logic {


//==============================================================================
// DuplicateImageFinderTest
//==============================================================================
struct DuplicateImageFinderTest : public ::testing::Test
{
	void SetUp() override
	{
		Initialize();
		m_pMockedCallback = callbacks::mocks::MockSearchProcessCallback::Create();
		ON_CALL(*m_pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(false));
		EXPECT_CALL(*m_pMockedCallback, onInit()).Times(1);
		EXPECT_CALL(*m_pMockedCallback, onFinish()).Times(1);
	}

	DuplicateImageFinder m_finder;
	callbacks::mocks::MockSearchProcessCallback::Ptr m_pMockedCallback;
};



//==============================================================================
// DuplicateImageFinderTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(DuplicateImageFinderTest, empty_input)
{
	FileSearchSettings settings;
	settings.sensitivity = 90;
	settings.processRotations = true;
	const std::vector<std::string> folders = {};

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);

	{
		InSequence s;
		for (unsigned int i = 1; i >= 3; ++i) {
		EXPECT_CALL(*m_pMockedCallback, setStage(i))
				.Times(1)
				.RetiresOnSaturation();
		}
	}

	EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 0)).Times(AtLeast(1));
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(_)).Times(0);
	EXPECT_EQ(0UL, m_finder.find(folders, settings, m_pMockedCallback).size());
}


//------------------------------------------------------------------------------
TEST_F(DuplicateImageFinderTest, images_80_rotations)
{
	FileSearchSettings settings;
	settings.sensitivity = 80;
	settings.processRotations = true;
	const std::vector<std::string> folders = { "data/images" };

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(_)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(2, _, _)).Times(2);

	// 2 lines below are exact duplicates specific
	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(_)).Times(0);

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(3)).Times(AtLeast(1)).RetiresOnSaturation();
	}

	for (int i = 0; i < 3; ++i)
	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 5)).Times(1).RetiresOnSaturation();;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(2, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(3, 5)).Times(1).RetiresOnSaturation();
		// last pass for is not called for 4th
		if (i != 0) {
			EXPECT_CALL(*m_pMockedCallback, updateProgress(4, 5)).Times(1).RetiresOnSaturation();
		}
		EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 5)).Times(1).RetiresOnSaturation();
	}

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(2UL, result.size());
	EXPECT_EQ(2UL, result[0].files.size());
	EXPECT_EQ(2UL, result[1].files.size());
	if (FileInfosContainFileName(result[0], "gm-125x80t.png"))
	{
		EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80t (copy).png"));
		EXPECT_TRUE(FileInfosContainFileName(result[1], "gm-125x80.gif"));
		EXPECT_TRUE(FileInfosContainFileName(result[1], "gm-125x80.png"));
	} else
	{
		EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.gif"));
		EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.png"));
		EXPECT_TRUE(FileInfosContainFileName(result[1], "gm-125x80t.png"));
		EXPECT_TRUE(FileInfosContainFileName(result[1], "gm-125x80t (copy).png"));
	}
}


//------------------------------------------------------------------------------
TEST_F(DuplicateImageFinderTest, images_70_rotations)
{
	FileSearchSettings settings;
	settings.sensitivity = 70;
	settings.processRotations = true;
	const std::vector<std::string> folders = { "data/images" };

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(_)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(1);

	// 2 lines below are exact duplicates specific
	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(_)).Times(0);

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(3)).Times(AtLeast(1)).RetiresOnSaturation();
	}

	for (int i = 0; i < 3; ++i)
	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 5)).Times(1).RetiresOnSaturation();;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(2, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(3, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(4, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 5)).Times(1).RetiresOnSaturation();
	}

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	EXPECT_EQ(5UL, result[0].files.size());

	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.gif"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.png"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80t.png"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80t (copy).png"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-654x418t.png"));
	for (const auto & fileInfo : result[0].files)
	{
		EXPECT_LT(0ULL, fileInfo.size);
		ASSERT_TRUE(fileInfo.imageResolution);
		EXPECT_LT(70UL, fileInfo.imageResolution->width);
		EXPECT_LT(70UL, fileInfo.imageResolution->height);
	}
}


//------------------------------------------------------------------------------
TEST_F(DuplicateImageFinderTest, images_70_no_rotations)
{
	FileSearchSettings settings;
	settings.sensitivity = 70;
	settings.processRotations = false;
	const std::vector<std::string> folders = { "data/images" };

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(_)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(1);

	// 2 lines below are exact duplicates specific
	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(_)).Times(0);

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(3)).Times(AtLeast(1)).RetiresOnSaturation();
	}

	for (int i = 0; i < 3; ++i)
	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 5)).Times(1).RetiresOnSaturation();;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(2, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(3, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(4, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 5)).Times(1).RetiresOnSaturation();
	}

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	EXPECT_EQ(4UL, result[0].files.size());

	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.gif"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.png"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80t.png"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-654x418t.png"));
}


//------------------------------------------------------------------------------
TEST_F(DuplicateImageFinderTest, images_default_settings)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = { "data/images" };

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(_)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(2, _, _)).Times(1);

	// 2 lines below are exact duplicates specific
	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(_)).Times(0);

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(3)).Times(AtLeast(1)).RetiresOnSaturation();
	}

	for (int i = 0; i < 3; ++i)
	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(2, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(3, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(4, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 5)).Times(1).RetiresOnSaturation();
	}

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	EXPECT_EQ(2UL, result[0].files.size());
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.gif"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.png"));
}


//------------------------------------------------------------------------------
TEST_F(DuplicateImageFinderTest, images_all_default)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = { "data" };

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(8);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(2, 5388, 2662)).Times(1);

	// 2 lines below are exact duplicates specific
	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(_)).Times(0);

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(AtLeast(1));//.RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(AtLeast(1));//.RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(3)).Times(AtLeast(1));//.RetiresOnSaturation();
	}

	for (int i = 0; i < 2; ++i)
	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(2, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(3, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(4, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 5)).Times(1).RetiresOnSaturation();
	}

	EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(2, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(3, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(4, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(6, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(7, 8)).Times(1).RetiresOnSaturation();
	EXPECT_CALL(*m_pMockedCallback, updateProgress(8, 8)).Times(1).RetiresOnSaturation();

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	EXPECT_EQ(2UL, result[0].files.size());
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.gif"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.png"));
}


//------------------------------------------------------------------------------
TEST_F(DuplicateImageFinderTest, images_all_regex)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = { "data" };
	settings.filenameRegexps = { ".*?/images/[^/]+$" };

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(3);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(2, 5388, 2662)).Times(1);

	// 2 lines below are exact duplicates specific
	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(_)).Times(0);

	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(AtLeast(1)).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, setStage(3)).Times(AtLeast(1)).RetiresOnSaturation();
	}

	for (int i = 0; i < 3; ++i)
	{
		InSequence s;
		EXPECT_CALL(*m_pMockedCallback, updateProgress(0, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(2, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(3, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(4, 5)).Times(1).RetiresOnSaturation();
		EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 5)).Times(1).RetiresOnSaturation();
	}

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	EXPECT_EQ(2UL, result[0].files.size());
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.gif"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "gm-125x80.png"));
}


}}
