/*
 ===============================================================================
 Name        : SearchProgressCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <atomic>
#include <callbacks/ControlCallback.hpp>
#include <glibmm.h>
#include <logic/callbacks/ISearchProcessCallback.hpp>
#include <logic/tools/StopTimer.hpp>

namespace detwinner::callbacks {

class SearchProgressCallback : public logic::callbacks::ISearchProcessCallback
{
public:
	using Ptr = std::shared_ptr<SearchProgressCallback>;
	static Ptr Create();

	void init() noexcept;
	SearchProgressCallback() noexcept;
	SearchProgressCallback(const SearchProgressCallback &) = delete;
	SearchProgressCallback & operator=(const SearchProgressCallback &) = delete;

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

	virtual void onFileProcessed(unsigned long long size) override;
	virtual void updateProgress(unsigned int progress, unsigned int total) override;
	virtual void
	onDuplicateFound(std::size_t numberOfFiles, unsigned long long totalSize, unsigned long long wastedSize) override;
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

	std::atomic_uint m_groupCount;
	std::atomic_uint m_duplicateCount;
	std::atomic_uint m_processedCount;
	std::atomic_uint m_totalCount;
	std::atomic_uint m_skippedCount;
	std::atomic_ullong m_processedSize;
	std::atomic_ullong m_duplicatesSize;
	std::atomic_ullong m_wastedSize;
	std::atomic_bool m_stage;
	std::atomic_bool m_paused;
	std::atomic_bool m_finished;
	Glib::Dispatcher m_dispatcher;
};

} // namespace detwinner::callbacks
