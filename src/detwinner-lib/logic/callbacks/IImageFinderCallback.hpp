/*
 ===============================================================================
 Name        : IImageFinderCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_CALLBACKS_IIMAGEFINDERCALLBACK_HPP_
#define LOGIC_CALLBACKS_IIMAGEFINDERCALLBACK_HPP_

#include <memory>


namespace detwinner {
namespace logic {
namespace callbacks {


struct IImageFinderCallback
{
	using Ptr_t = std::shared_ptr<IImageFinderCallback>;
	virtual void imgIndexingProgress(int, int) = 0;
	virtual void imgComparingProgress(int, int) = 0;
	virtual void imgOrganizingProgress(int, int) = 0;
	virtual void similarImagesFound(std::size_t, unsigned long long, unsigned long long) = 0;
	virtual bool pauseAndStopStatus() = 0;
	virtual ~IImageFinderCallback() noexcept = default;
};


}}}

#endif /* LOGIC_CALLBACKS_IIMAGEFINDERCALLBACK_HPP_ */
