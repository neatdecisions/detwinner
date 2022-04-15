/*
 ===============================================================================
 Name        : ImageFinderCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */


#ifndef LOGIC_CALLBACKS_IMAGEFINDERCALLBACK_HPP_
#define LOGIC_CALLBACKS_IMAGEFINDERCALLBACK_HPP_

#include <logic/callbacks/ISearchProcessCallback.hpp>
#include <logic/callbacks/IImageFinderCallback.hpp>
#include <atomic>


namespace detwinner {
namespace logic {
namespace callbacks {


//==============================================================================
// ImageFinderCallback
//==============================================================================
class ImageFinderCallback: public IImageFinderCallback
{
public:
	explicit ImageFinderCallback(const ISearchProcessCallback::Ptr_t & callback);
	virtual void imgIndexingProgress(int, int) override;
	virtual void imgComparingProgress(int current, int total) override;
	virtual void imgOrganizingProgress(int current, int total) override;
	virtual void similarImagesFound(
		std::size_t fileCount,
		unsigned long long totalSize,
		unsigned long long wastedSize) override;
	virtual bool pauseAndStopStatus() override;

protected:
	ISearchProcessCallback::Ptr_t m_callback;
	std::atomic_bool m_compareStarted;
};


}}}

#endif /* LOGIC_CALLBACKS_IMAGEFINDERCALLBACK_HPP_ */
