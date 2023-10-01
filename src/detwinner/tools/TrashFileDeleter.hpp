/*
 ===============================================================================
 Name        : TrashFileDeleter.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <tools/AbstractFileDeleter.hpp>

namespace detwinner::tools {

struct TrashFileDeleter : AbstractFileDeleter
{
	virtual bool removeFile(const std::string & filePath) override;
};

} // namespace detwinner::tools
