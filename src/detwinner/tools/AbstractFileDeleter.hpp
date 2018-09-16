/*
 ===============================================================================
 Name        : AbstractFileDeleter.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef TOOLS_ABSTRACTFILEDELETER_HPP_
#define TOOLS_ABSTRACTFILEDELETER_HPP_

#include <memory>
#include <string>


namespace detwinner {
namespace tools {


class AbstractFileDeleter
{
public:
	using Ptr_t = std::shared_ptr<AbstractFileDeleter>;
	AbstractFileDeleter() = default;
	virtual ~AbstractFileDeleter() noexcept = default;
	virtual bool removeFile(const std::string & filePath) = 0;
};


}}

#endif /* TOOLS_ABSTRACTFILEDELETER_HPP_ */
