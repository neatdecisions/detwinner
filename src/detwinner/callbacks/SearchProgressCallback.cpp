/*
 ============================================================================
 Name        : SearchProgressCallback.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */


#include <callbacks/SearchProgressCallback.hpp>


namespace detwinner {
namespace callbacks {


//------------------------------------------------------------------------------
SearchProgressCallback::Ptr_t
SearchProgressCallback::Create()
{
	return std::make_shared<SearchProgressCallback>();
}


//------------------------------------------------------------------------------
long long
SearchProgressCallback::getElapsedTime() const
{
	return m_timer.elapsed();
}


//------------------------------------------------------------------------------
unsigned int
SearchProgressCallback::getGroupCount() const
{
	return m_groupCount;
}


//------------------------------------------------------------------------------
unsigned int
SearchProgressCallback::getDuplicateCount() const
{
	return m_duplicateCount;
}


//------------------------------------------------------------------------------
unsigned int
SearchProgressCallback::getProcessedCount() const
{
	return m_processedCount;
}


//------------------------------------------------------------------------------
unsigned int
SearchProgressCallback::getTotalCount() const
{
	return m_totalCount;
}


//------------------------------------------------------------------------------
unsigned int
SearchProgressCallback::getSkippedCount() const
{
	return m_skippedCount;
}


//------------------------------------------------------------------------------
unsigned long long
SearchProgressCallback::getProcessedSize() const
{
	return m_processedSize;
}


//------------------------------------------------------------------------------
unsigned long long
SearchProgressCallback::getDuplicatesSize() const
{
	return m_duplicatesSize;
}


//------------------------------------------------------------------------------
unsigned long long
SearchProgressCallback::getWastedSize() const
{
	return m_wastedSize;
}


//------------------------------------------------------------------------------
int
SearchProgressCallback::getStage() const
{
	return m_stage;
}


//------------------------------------------------------------------------------
bool
SearchProgressCallback::isPaused() const
{
	return m_paused;
}


//------------------------------------------------------------------------------
bool
SearchProgressCallback::isFinished() const
{
	return m_finished;
}


//------------------------------------------------------------------------------
Glib::Dispatcher &
SearchProgressCallback::accessDispatcher()
{
	return m_dispatcher;
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::onFileProcessed(const unsigned long long)
{
	++m_processedCount;
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::updateProgress(unsigned int progress, unsigned int total)
{
	m_processedCount = progress;
	m_totalCount = total;
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::onDuplicateFound(
	const std::size_t numberOfFiles,
	const unsigned long long totalSize,
	const unsigned long long wastedSize)
{
	m_duplicatesSize += totalSize;
	m_wastedSize += wastedSize;
	++m_groupCount;
	m_duplicateCount += numberOfFiles;
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::onStartComparing(unsigned int totalNumber)
{
	m_totalCount = totalNumber;
	m_processedCount = 0;
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::onFileIndexed(bool skipped)
{
	if (skipped)
	{
		++m_skippedCount;
	} else
	{
		++m_totalCount;
	}
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::setStage(int stage)
{
	m_stage = stage;
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::onFinish()
{
	g_info("Elapsed time: %s", Glib::DateTime::create_now_utc(static_cast<gint64>(m_timer.elapsed())).format("%H∶%M∶%S").c_str());
	m_timer.stop();
	m_finished = true;
	m_dispatcher.emit();
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::onInit()
{
	m_stage = 0;
}


//------------------------------------------------------------------------------
bool
SearchProgressCallback::pauseAndStopStatus()
{
	return m_pauser.pauseAndStopStatus();
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::togglePause()
{
	m_paused = !m_paused;
	// m_refreshConnection.block(m_progressItem->paused);   ??? doesn't work as expected
	m_pauser.pause(m_paused);
	if (m_paused)
	{
		m_timer.stop();
	} else
	{
		m_timer.start();
	}
}


//------------------------------------------------------------------------------
void
SearchProgressCallback::stop()
{
	m_timer.stop();
	m_pauser.stop();
}


}}
