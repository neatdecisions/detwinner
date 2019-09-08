/*
 ===============================================================================
 Name        : StopTimer.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */


#include <logic/tools/StopTimer.hpp>


namespace detwinner {
namespace logic {
namespace tools {


//------------------------------------------------------------------------------
StopTimer::StopTimer() :
	m_startTime(std::chrono::steady_clock::now())
{}


//------------------------------------------------------------------------------
void
StopTimer::start()
{
	m_stopped = false;
	m_startTime = std::chrono::steady_clock::now();
}


//------------------------------------------------------------------------------
void
StopTimer::stop()
{
	if (!m_stopped)
	{
		m_elapsed += std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_startTime).count();
		m_stopped = true;
	}
}


//------------------------------------------------------------------------------
long long
StopTimer::elapsed() const
{
	return m_stopped ?
		m_elapsed :
		m_elapsed + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_startTime).count();
}


}}}
