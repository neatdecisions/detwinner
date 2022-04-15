/*
 ===============================================================================
 Name        : ISearchProcessCallback.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */


#ifndef LOGIC_CALLBACKS_ISEARCHPROCESSCALLBACK_HPP_
#define LOGIC_CALLBACKS_ISEARCHPROCESSCALLBACK_HPP_

#include <memory>


namespace detwinner {
namespace logic {
namespace callbacks {


class ISearchProcessCallback
{
public:
	using Ptr_t = std::shared_ptr<ISearchProcessCallback>;
	virtual ~ISearchProcessCallback() noexcept = default;

	virtual void onFileProcessed(unsigned long long size) = 0;
	virtual void onDuplicateFound(
		std::size_t numberOfFiles,
		unsigned long long totalSize,
		unsigned long long wastedSize) = 0;
	virtual void onStartComparing(unsigned int totalNumber) = 0;
	virtual void onFileIndexed(bool skipped) = 0;
	virtual void onInit() = 0;
	virtual void onFinish() = 0;
	virtual void setStage(int stage) = 0;
	virtual void updateProgress(unsigned int progress, unsigned int total) = 0;
	virtual bool pauseAndStopStatus() = 0;
};

}}}

#endif /* LOGIC_CALLBACKS_ISEARCHPROCESSCALLBACK_HPP_ */
