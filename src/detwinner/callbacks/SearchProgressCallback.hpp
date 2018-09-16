/*
 ===============================================================================
 Name        : SearchProgressCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */


#ifndef CALLBACKS_SEARCHPROGRESSCALLBACK_HPP_
#define CALLBACKS_SEARCHPROGRESSCALLBACK_HPP_

#include <callbacks/ControlCallback.hpp>
#include <logic/callbacks/ISearchProcessCallback.hpp>
#include <logic/tools/StopTimer.hpp>
#include <atomic>
#include <glibmm.h>


namespace detwinner {
namespace callbacks {


struct SearchProgressCallback : public logic::callbacks::ISearchProcessCallback
{
public:
	using Ptr_t = std::shared_ptr<SearchProgressCallback>;
	static Ptr_t Create();

	unsigned int getGroupCount() const;
	unsigned int getDuplicateCount() const;
	unsigned int getProcessedCount() const;
	unsigned int getTotalCount() const;
	unsigned int getSkippedCount() const;
	unsigned long long getProcessedSize() const;
	unsigned long long getDuplicatesSize() const;
	unsigned long long getWastedSize() const;
	int getStage() const;
	bool isPaused() const;
	bool isFinished() const;
	Glib::Dispatcher & accessDispatcher();
	long long getElapsedTime() const;

	SearchProgressCallback() : m_groupCount(0), m_duplicateCount(0), m_processedCount(0),
			m_totalCount(0), m_skippedCount(0), m_processedSize(0), m_duplicatesSize(0),
			m_wastedSize(0), m_stage(0), m_paused(false), m_finished(false)
	{}

	void init()
	{
		m_duplicateCount = 0;
		m_groupCount = 0;
		m_processedCount = 0;
		m_processedSize = 0;
		m_skippedCount = 0;
		m_totalCount = 0;
		m_duplicatesSize = 0;
		m_wastedSize = 0;
		m_stage = 0;
		m_paused = false;
		m_finished = false;
	}

	void onFileProcessed(const unsigned long long size) override;
	void updateProgress(unsigned int progress, unsigned int total) override;
	virtual void onDuplicateFound(
		const std::size_t numberOfFiles,
		const unsigned long long totalSize,
		const unsigned long long wastedSize) override;
	void onStartComparing(unsigned int totalNumber) override;
	void onFileIndexed(bool skipped) override;
	void setStage(int stage) override;
	void onFinish() override;
	void onInit() override;
	bool pauseAndStopStatus() override;

	void togglePause();
	void stop();

private:
	callbacks::ControlCallback m_pauser;
	logic::tools::StopTimer m_timer;

	std::atomic<unsigned int> m_groupCount;
	std::atomic<unsigned int> m_duplicateCount;
	std::atomic<unsigned int> m_processedCount;
	std::atomic<unsigned int> m_totalCount;
	std::atomic<unsigned int> m_skippedCount;
	std::atomic<unsigned long long> m_processedSize;
	std::atomic<unsigned long long> m_duplicatesSize;
	std::atomic<unsigned long long> m_wastedSize;
	std::atomic<int> m_stage;
	std::atomic<bool> m_paused;
	std::atomic<bool> m_finished;
	Glib::Dispatcher m_dispatcher;
};


}}

#endif /* CALLBACKS_SEARCHPROGRESSCALLBACK_HPP_ */
