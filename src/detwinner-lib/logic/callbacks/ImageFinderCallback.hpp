/*
 ===============================================================================
 Name        : ImageFinderCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <atomic>

#include <logic/callbacks/IImageFinderCallback.hpp>
#include <logic/callbacks/ISearchProcessCallback.hpp>

namespace detwinner::logic::callbacks {

//==============================================================================
// ImageFinderCallback
//==============================================================================
class ImageFinderCallback : public IImageFinderCallback
{
public:
	explicit ImageFinderCallback(const ISearchProcessCallback::Ptr & callback);
	virtual void imgIndexingProgress(int current, int total) override;
	virtual void imgComparingProgress(int current, int total) override;
	virtual void imgOrganizingProgress(int current, int total) override;
	virtual void
	similarImagesFound(std::size_t fileCount, unsigned long long totalSize, unsigned long long wastedSize) override;
	virtual bool pauseAndStopStatus() override;

protected:
	ISearchProcessCallback::Ptr m_callback;
	std::atomic_bool m_compareStarted;
};

} // namespace detwinner::logic::callbacks