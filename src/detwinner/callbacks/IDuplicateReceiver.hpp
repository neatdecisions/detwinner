/*
 ===============================================================================
 Name        : IDuplicateReceiver.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef CALLBACKS_IDUPLICATERECEIVER_HPP_
#define CALLBACKS_IDUPLICATERECEIVER_HPP_

#include <callbacks/IDeferredAction.hpp>
#include <logic/CommonDataTypes.hpp>


namespace detwinner {
namespace callbacks {


class IDuplicateReceiver
{
public:
	using Ptr_t = std::shared_ptr<IDuplicateReceiver>;
	virtual IDeferredAction::Ptr_t populate(logic::DuplicatesList_t && container) = 0;
	virtual ~IDuplicateReceiver() noexcept = default;
};

}}

#endif /* CALLBACKS_IDUPLICATERECEIVER_HPP_ */
