/*
 ===============================================================================
 Name        : ImageFinderCallback.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/callbacks/ImageFinderCallback.hpp>


namespace detwinner {
namespace logic {
namespace callbacks {


//------------------------------------------------------------------------------
ImageFinderCallback::ImageFinderCallback(const ISearchProcessCallback::Ptr_t & callback) :
		m_callback(callback), m_compareStarted(false)
{}


//------------------------------------------------------------------------------
void
ImageFinderCallback::imgIndexingProgress(int current, int total)
{
	if (!m_callback) return;
	m_callback->setStage(1);
	m_callback->updateProgress(current, total);
}


//------------------------------------------------------------------------------
void
ImageFinderCallback::imgComparingProgress(int current, int total)
{
	if (!m_callback) return;
	m_callback->setStage(2);
	m_callback->updateProgress(current, total);
}


//------------------------------------------------------------------------------
void
ImageFinderCallback::imgOrganizingProgress(int current, int total)
{
	if (!m_callback) return;
	m_callback->setStage(3);
	m_callback->updateProgress(current, total);
}


//------------------------------------------------------------------------------
void
ImageFinderCallback::similarImagesFound(
	std::size_t fileCount,
	unsigned long long totalSize,
	unsigned long long wastedSize)
{
	if ( m_callback && (fileCount > 0) )
	{
		m_callback->onDuplicateFound(fileCount, totalSize, wastedSize);
	}
}


//------------------------------------------------------------------------------
bool
ImageFinderCallback::pauseAndStopStatus()
{
	return m_callback ? m_callback->pauseAndStopStatus() : false;
}


}}}
