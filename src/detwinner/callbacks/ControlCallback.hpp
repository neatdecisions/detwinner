/*
 ===============================================================================
 Name        : ControlCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef CALLBACKS_CONTROLCALLBACK_HPP_
#define CALLBACKS_CONTROLCALLBACK_HPP_

#include <atomic>
#include <condition_variable>
#include <mutex>


namespace detwinner {
namespace callbacks {


class ControlCallback
{
public:
	ControlCallback();
	virtual ~ControlCallback() noexcept = default;
	bool isPaused() const;
	void pause(bool doPause = true);
	void stop();
	virtual bool pauseAndStopStatus();

private:
	std::condition_variable m_pauseEvent;
	std::mutex m_mutex;
	std::atomic<bool> m_isStopped;
	std::atomic<bool> m_isPaused;
};


}}

#endif /* CALLBACKS_CONTROLCALLBACK_HPP_ */
