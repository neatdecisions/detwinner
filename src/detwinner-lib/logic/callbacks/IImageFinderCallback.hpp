/*
 ===============================================================================
 Name        : IImageFinderCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <memory>

namespace detwinner::logic::callbacks {

struct IImageFinderCallback
{
	using Ptr = std::shared_ptr<IImageFinderCallback>;
	virtual void imgIndexingProgress(int current, int total) = 0;
	virtual void imgComparingProgress(int current, int total) = 0;
	virtual void imgOrganizingProgress(int current, int total) = 0;
	virtual void
	similarImagesFound(std::size_t fileCount, unsigned long long totalSize, unsigned long long wastedSize) = 0;
	virtual bool pauseAndStopStatus() = 0;
	virtual ~IImageFinderCallback() noexcept = default;
};

} // namespace detwinner::logic::callbacks
