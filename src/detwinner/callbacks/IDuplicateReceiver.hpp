/*
 ===============================================================================
 Name        : IDuplicateReceiver.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <callbacks/IDeferredAction.hpp>
#include <logic/CommonDataTypes.hpp>

namespace detwinner::callbacks {

struct IDuplicateReceiver
{
	using Ptr = std::shared_ptr<IDuplicateReceiver>;
	virtual IDeferredAction::Ptr populate(logic::DuplicatesList && container) = 0;
	virtual ~IDuplicateReceiver() noexcept = default;
};

} // namespace detwinner::callbacks
