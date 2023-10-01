#include <gtest/gtest.h>

#include <logic/callbacks/ImageFinderCallback.hpp>

#include "../mocks/MockSearchProcessCallback.hpp"

using ::testing::_;
using ::testing::Return;

namespace detwinner::logic::callbacks {

//==============================================================================
// ImageFinderCallbackTest
//==============================================================================
struct ImageFinderCallbackTest : public ::testing::Test
{
	void SetUp() override
	{
		m_pMockedCallback = mocks::MockSearchProcessCallback::Create();
		EXPECT_CALL(*m_pMockedCallback, onInit()).Times(0);
		EXPECT_CALL(*m_pMockedCallback, onFinish()).Times(0);
		EXPECT_CALL(*m_pMockedCallback, onFileProcessed(_)).Times(0);
		EXPECT_CALL(*m_pMockedCallback, onStartComparing(0)).Times(0);
	}

	mocks::MockSearchProcessCallback::Ptr m_pMockedCallback;
};

//==============================================================================
// ImageFinderCallbackTest - fixtures
//==============================================================================

//------------------------------------------------------------------------------
TEST_F(ImageFinderCallbackTest, imgIndexingProgress)
{
	ImageFinderCallback imageCallback(m_pMockedCallback);
	EXPECT_CALL(*m_pMockedCallback, setStage(1)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, updateProgress(5, 6)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, pauseAndStopStatus()).Times(0);
	imageCallback.imgIndexingProgress(5, 6);
}

//------------------------------------------------------------------------------
TEST_F(ImageFinderCallbackTest, imgComparingProgress)
{
	ImageFinderCallback imageCallback(m_pMockedCallback);
	EXPECT_CALL(*m_pMockedCallback, setStage(2)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, updateProgress(1, 2)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, pauseAndStopStatus()).Times(0);
	imageCallback.imgComparingProgress(1, 2);
}

//------------------------------------------------------------------------------
TEST_F(ImageFinderCallbackTest, similarImagesFound_normal)
{
	ImageFinderCallback imageCallback(m_pMockedCallback);
	EXPECT_CALL(*m_pMockedCallback, setStage(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, updateProgress(_, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(5, 10, 11)).Times(1);
	EXPECT_CALL(*m_pMockedCallback, pauseAndStopStatus()).Times(0);
	imageCallback.similarImagesFound(5, 10, 11);
}

//------------------------------------------------------------------------------
TEST_F(ImageFinderCallbackTest, similarImagesFound_none)
{
	ImageFinderCallback imageCallback(m_pMockedCallback);
	EXPECT_CALL(*m_pMockedCallback, setStage(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, updateProgress(_, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, pauseAndStopStatus()).Times(0);
	imageCallback.similarImagesFound(0, 10, 11);
}

//------------------------------------------------------------------------------
TEST_F(ImageFinderCallbackTest, pauseAndStopStatus_true)
{
	ImageFinderCallback imageCallback(m_pMockedCallback);
	EXPECT_CALL(*m_pMockedCallback, setStage(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, updateProgress(_, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, pauseAndStopStatus()).WillOnce(Return(true));
	EXPECT_TRUE(imageCallback.pauseAndStopStatus());
}

//------------------------------------------------------------------------------
TEST_F(ImageFinderCallbackTest, pauseAndStopStatus_false)
{
	ImageFinderCallback imageCallback(m_pMockedCallback);
	EXPECT_CALL(*m_pMockedCallback, setStage(_)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, updateProgress(_, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, onDuplicateFound(_, _, _)).Times(0);
	EXPECT_CALL(*m_pMockedCallback, pauseAndStopStatus()).WillOnce(Return(false));
	EXPECT_FALSE(imageCallback.pauseAndStopStatus());
}

//------------------------------------------------------------------------------
TEST_F(ImageFinderCallbackTest, null_base_callback)
{
	ImageFinderCallback imageCallback(nullptr);
	EXPECT_NO_FATAL_FAILURE(imageCallback.imgIndexingProgress(1, 2));
	EXPECT_NO_FATAL_FAILURE(imageCallback.imgComparingProgress(1, 2));
	EXPECT_NO_FATAL_FAILURE(imageCallback.imgOrganizingProgress(1, 2));
	EXPECT_NO_FATAL_FAILURE(imageCallback.similarImagesFound(1, 2, 3));
	EXPECT_NO_FATAL_FAILURE(imageCallback.pauseAndStopStatus());
}

} // namespace detwinner::logic::callbacks
