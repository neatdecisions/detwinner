#include <gtest/gtest.h>

#include <logic/DuplicateFilesFinder.hpp>

#include "mocks/MockSearchProcessCallback.hpp"
#include "TestingHelpers.hpp"


using ::testing::Return;
using ::testing::_;


namespace detwinner {
namespace logic {


//==============================================================================
// DuplicateFilesFinderTest
//==============================================================================
struct DuplicateFilesFinderTest : public ::testing::Test
{
	void SetUp() override
	{
		m_pMockedCallback = callbacks::mocks::MockSearchProcessCallback::Create();
		ON_CALL(*m_pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(false));
		EXPECT_CALL(*m_pMockedCallback, onInit()).Times(1);
		EXPECT_CALL(*m_pMockedCallback, onFinish()).Times(1);
		EXPECT_CALL(*m_pMockedCallback, updateProgress(_, _)).Times(0);
		EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(1);
		EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(0);
		EXPECT_CALL(*m_pMockedCallback, setStage(3)).Times(0);
	}

	DuplicateFilesFinder m_finder;
	callbacks::mocks::MockSearchProcessCallback::Ptr m_pMockedCallback;
};



//==============================================================================
// ImageFeaturesTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(DuplicateFilesFinderTest, empty_input)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = {};

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onFolderSkipped()).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(0)).Times(1);

	EXPECT_EQ(0UL, m_finder.find(folders, settings, m_pMockedCallback).size());
}


//------------------------------------------------------------------------------
TEST_F(DuplicateFilesFinderTest, basic_files)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = { "data/files" };

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(82)).Times(2);
	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(0)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(3);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(2, 164, 82)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(3)).Times(1);

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	EXPECT_EQ(2UL, result[0].files.size());
	EXPECT_TRUE(FileInfosContainFileName(result[0], "file1.txt"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "file2.txt"));
}


//------------------------------------------------------------------------------
TEST_F(DuplicateFilesFinderTest, basic_images)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = { "data/images" };

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(5)).Times(1);

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	EXPECT_EQ(0UL, result.size());
}


//------------------------------------------------------------------------------
TEST_F(DuplicateFilesFinderTest, basic_all)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = { "data" };

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(8);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(8);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(2, 164, 82)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(8)).Times(1);

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	EXPECT_EQ(2UL, result[0].files.size());
	EXPECT_TRUE(FileInfosContainFileName(result[0], "file1.txt"));
	EXPECT_TRUE(FileInfosContainFileName(result[0], "file2.txt"));
}


//------------------------------------------------------------------------------
TEST_F(DuplicateFilesFinderTest, basic_regex_positive)
{
	FileSearchSettings settings;
	settings.filenameRegexps = { ".*?/file\\d\\.txt(\\.[^.]*$|$)" };
	const std::vector<std::string> folders = { "data" };

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(82)).Times(2);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(6);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(2);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(2, 164, 82)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(2)).Times(1);

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(1UL, result.size());
	ASSERT_EQ(2UL, result[0].files.size());
	EXPECT_TRUE(FileInfosContainFileName(result[0], "file1.txt"));
	EXPECT_EQ(82ULL, result[0].files[0].size);
	EXPECT_FALSE(result[0].files[0].imageResolution);
	EXPECT_TRUE(FileInfosContainFileName(result[0], "file2.txt"));
	EXPECT_EQ(82ULL, result[0].files[1].size);
	EXPECT_FALSE(result[0].files[1].imageResolution);
}


//------------------------------------------------------------------------------
TEST_F(DuplicateFilesFinderTest, basic_regex_negative)
{
	FileSearchSettings settings;
	settings.filenameRegexps = { ".*?/foobar\\d\\.txt(\\.[^.]*$|$)" };
	const std::vector<std::string> folders = { "data" };

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(8);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onStartComparing(0)).Times(1);

	const DuplicatesList_t result = m_finder.find(folders, settings, m_pMockedCallback);
	ASSERT_EQ(0UL, result.size());
}


}}
