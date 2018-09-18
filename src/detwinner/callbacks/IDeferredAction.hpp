/*
 ===============================================================================
 Name        : IDeferredAction.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef CALLBACKS_IDEFERREDACTION_HPP_
#define CALLBACKS_IDEFERREDACTION_HPP_

#include <memory>


namespace detwinner {
namespace callbacks {


struct IDeferredAction
{
	enum class Result_t
	{
		Unknown,
		Mixed,
		Failure,
		Success
	};
	using Ptr_t = std::shared_ptr<IDeferredAction>;
	virtual double getProgress() const = 0;
	virtual bool processNext() = 0;
	virtual Result_t getStatus() const = 0;
	virtual ~IDeferredAction() noexcept = default;
};


}}

#endif /* CALLBACKS_IDEFERREDACTION_HPP_ */
