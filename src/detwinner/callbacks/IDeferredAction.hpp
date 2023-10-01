/*
 ===============================================================================
 Name        : IDeferredAction.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <memory>

namespace detwinner::callbacks {

struct IDeferredAction
{
	enum class Result { Unknown, Mixed, Failure, Success };
	using Ptr = std::shared_ptr<IDeferredAction>;
	virtual double getProgress() const = 0;
	virtual bool processNext() = 0;
	virtual Result getStatus() const = 0;
	virtual ~IDeferredAction() noexcept = default;
};

} // namespace detwinner::callbacks
