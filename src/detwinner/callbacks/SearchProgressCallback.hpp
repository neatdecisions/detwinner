/*
 ===============================================================================
 Name        : SearchProgressCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
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

	void init() noexcept;
	SearchProgressCallback() noexcept;
	SearchProgressCallback(const SearchProgressCallback&) = delete;
	SearchProgressCallback& operator=(const SearchProgressCallback&) = delete;

	unsigned int getGroupCount() const noexcept;
	unsigned int getDuplicateCount() const noexcept;
	unsigned int getProcessedCount() const noexcept;
	unsigned int getTotalCount() const noexcept;
	unsigned int getSkippedCount() const noexcept;
	unsigned long long getProcessedSize() const noexcept;
	unsigned long long getDuplicatesSize() const noexcept;
	unsigned long long getWastedSize() const noexcept;
	int getStage() const noexcept;
	bool isPaused() const noexcept;
	bool isFinished() const noexcept;
	Glib::Dispatcher & accessDispatcher() noexcept;
	long long getElapsedTime() const;


	virtual void onFileProcessed(const unsigned long long size) override;
	virtual void updateProgress(unsigned int progress, unsigned int total) override;
	virtual void onDuplicateFound(
		const std::size_t numberOfFiles,
		const unsigned long long totalSize,
		const unsigned long long wastedSize) override;
	virtual void onStartComparing(unsigned int totalNumber) override;
	virtual void onFileIndexed(bool skipped) override;
	virtual void setStage(int stage) override;
	virtual void onFinish() override;
	virtual void onInit() override;
	virtual bool pauseAndStopStatus() override;

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
