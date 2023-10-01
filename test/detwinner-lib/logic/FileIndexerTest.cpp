#include <gtest/gtest.h>

#include <logic/FileIndexer.hpp>

#include "TestingHelpers.hpp"
#include "mocks/MockSearchProcessCallback.hpp"

using ::testing::_;
using ::testing::Return;

namespace detwinner::logic {

//==============================================================================
// DuplicateImageFinderTest
//==============================================================================
struct FileIndexerTest : public ::testing::Test
{
	void SetUp() override
	{
		m_pMockedCallback = callbacks::mocks::MockSearchProcessCallback::Create();
		ON_CALL(*m_pMockedCallback, pauseAndStopStatus()).WillByDefault(Return(false));
		EXPECT_CALL(*m_pMockedCallback, onInit()).Times(0);
		EXPECT_CALL(*m_pMockedCallback, onFinish()).Times(0);
		EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
		EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
		EXPECT_CALL(*m_pMockedCallback, setStage(_)).Times(0);
		EXPECT_CALL(*m_pMockedCallback, onStartComparing(0)).Times(0);
		EXPECT_CALL(*m_pMockedCallback, updateProgress(_, _)).Times(0);
	}

	callbacks::mocks::MockSearchProcessCallback::Ptr m_pMockedCallback;
};

//==============================================================================
// FakeFileReceiver
//==============================================================================
struct FakeFileReceiver : callbacks::IIndexedFileReceiver
{
	void receive(FileInfo && fileInfo) override { fileNames.push_back(std::move(fileInfo.fullPath)); }
	std::vector<std::string> fileNames;
};

//==============================================================================
// FileIndexerTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, basic_data)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;

	EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(8);

	FileIndexer(settings).performIndexing(folders, fileReceiver, m_pMockedCallback);

	EXPECT_EQ(8UL, fileReceiver.fileNames.size());
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "file1.txt"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "file2.txt"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "empty.foo"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.gif"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t (copy).png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-654x418t.png"));
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, size_greater_than)
{
	FileSearchSettings settings;
	settings.minFileSize = 82ULL;
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(3);

	FileIndexer(settings).performIndexing(folders, fileReceiver, m_pMockedCallback);

	EXPECT_EQ(5UL, fileReceiver.fileNames.size());
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.gif"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t (copy).png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-654x418t.png"));
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, size_smaller_than)
{
	FileSearchSettings settings;
	settings.maxFileSize = 82ULL;
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(7);

	FileIndexer(settings).performIndexing(folders, fileReceiver, m_pMockedCallback);

	EXPECT_EQ(1UL, fileReceiver.fileNames.size());
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "empty.foo"));
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, size_in_between)
{
	FileSearchSettings settings;
	settings.minFileSize = 82ULL;
	settings.maxFileSize = 3000ULL;
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(2);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(6);

	FileIndexer(settings).performIndexing(folders, fileReceiver, m_pMockedCallback);

	EXPECT_EQ(2UL, fileReceiver.fileNames.size());
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.gif"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.png"));
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, regex_wrong)
{
	FileSearchSettings settings;
	settings.filenameRegexps = {"?"};
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(8);

	FileIndexer(settings).performIndexing(folders, fileReceiver, m_pMockedCallback);

	EXPECT_EQ(8UL, fileReceiver.fileNames.size());
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "file1.txt"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "file2.txt"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "empty.foo"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.gif"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t (copy).png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-654x418t.png"));
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, regex_single)
{
	FileSearchSettings settings;
	settings.filenameRegexps = {".*?gm.*?"};
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(5);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(3);

	FileIndexer(settings).performIndexing(folders, fileReceiver, m_pMockedCallback);

	EXPECT_EQ(5UL, fileReceiver.fileNames.size());
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.gif"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80t (copy).png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-654x418t.png"));
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, regex_multiple)
{
	FileSearchSettings settings;
	settings.filenameRegexps = {".*?file\\d\\.txt", ".*?80\\.gif", ".*?80\\.png"};
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;

	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(false)).Times(4);
	EXPECT_CALL(*m_pMockedCallback, onFileIndexed(true)).Times(4);

	FileIndexer(settings).performIndexing(folders, fileReceiver, m_pMockedCallback);

	EXPECT_EQ(4UL, fileReceiver.fileNames.size());
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.gif"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "gm-125x80.png"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "file1.txt"));
	EXPECT_TRUE(FilePathsContainFileName(fileReceiver.fileNames, "file2.txt"));
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, interruption)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = {"data"};
	auto pMockedCallback = callbacks::mocks::MockSearchProcessCallback::Create();
	EXPECT_CALL(*pMockedCallback, pauseAndStopStatus()).WillOnce(Return(true));
	FakeFileReceiver fileReceiver;
	FileIndexer(settings).performIndexing(folders, fileReceiver, pMockedCallback);

	EXPECT_TRUE(fileReceiver.fileNames.empty());
}

//------------------------------------------------------------------------------
TEST_F(FileIndexerTest, null_callback)
{
	FileSearchSettings settings;
	const std::vector<std::string> folders = {"data"};
	FakeFileReceiver fileReceiver;
	FileIndexer(settings).performIndexing(folders, fileReceiver, nullptr);

	EXPECT_EQ(8UL, fileReceiver.fileNames.size());
}

} // namespace detwinner::logic
