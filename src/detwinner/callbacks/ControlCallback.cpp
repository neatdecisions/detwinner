/*
 ===============================================================================
 Name        : ControlCallback.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <callbacks/ControlCallback.hpp>


namespace detwinner {
namespace callbacks {


//------------------------------------------------------------------------------
ControlCallback::ControlCallback() :
	m_isStopped(false),
	m_isPaused(false)
{}


//------------------------------------------------------------------------------
bool
ControlCallback::isPaused() const
{
	return m_isPaused;
}


//------------------------------------------------------------------------------
void
ControlCallback::pause(bool doPause)
{
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_isPaused = doPause;
	}
	m_pauseEvent.notify_all();
}


//------------------------------------------------------------------------------
void
ControlCallback::stop()
{
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_isStopped = true;
		m_isPaused = false;
	}
	m_pauseEvent.notify_all();
}


//------------------------------------------------------------------------------
bool
ControlCallback::pauseAndStopStatus()
{
	std::unique_lock<std::mutex> lk(m_mutex);
	m_pauseEvent.wait(lk, [this]{return !m_isPaused;});
	lk.unlock();
	return m_isStopped;
}


}}

