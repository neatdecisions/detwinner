/*
 ===============================================================================
 Name        : StopTimer.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_TOOLS_STOPTIMER_HPP_
#define LOGIC_TOOLS_STOPTIMER_HPP_

#include <chrono>


namespace detwinner {
namespace logic {
namespace tools {


//==============================================================================
// StopTimer
//==============================================================================
class StopTimer
{
public:
	StopTimer();
	void start();
	void stop();
	long long elapsed() const;

private:
	long long m_elapsed;
	std::chrono::time_point<std::chrono::steady_clock> m_startTime;
	bool m_stopped;
};


}}}

#endif /* LOGIC_TOOLS_STOPTIMER_HPP_ */
