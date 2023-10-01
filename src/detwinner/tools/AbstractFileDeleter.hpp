/*
 ===============================================================================
 Name        : AbstractFileDeleter.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <memory>
#include <string>

namespace detwinner::tools {

struct AbstractFileDeleter
{
	using Ptr = std::shared_ptr<AbstractFileDeleter>;
	AbstractFileDeleter() = default;
	virtual ~AbstractFileDeleter() noexcept = default;
	virtual bool removeFile(const std::string & filePath) = 0;
};

} // namespace detwinner::tools
