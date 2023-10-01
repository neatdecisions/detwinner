/*
 ===============================================================================
 Name        : ControlCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace detwinner::callbacks {

class ControlCallback
{
public:
	ControlCallback();
	bool isPaused() const;
	void pause(bool doPause = true);
	void stop();
	bool pauseAndStopStatus();

private:
	std::condition_variable m_pauseEvent;
	std::mutex m_mutex;
	std::atomic_bool m_isStopped;
	std::atomic_bool m_isPaused;
};

} // namespace detwinner::callbacks
