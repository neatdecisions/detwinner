/*
 ===============================================================================
 Name        : FileSearchSettings.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace detwinner::logic {

struct FileSearchSettings
{
	std::optional<unsigned short> sensitivity;
	std::optional<bool> processRotations;
	std::optional<unsigned long long> minFileSize;
	std::optional<unsigned long long> maxFileSize;
	std::vector<std::string> filenameRegexps;
	bool searchReadOnlyFiles = true;
	bool searchHiddenFiles = false;
	bool searchExecutableFiles = false;
};

} // namespace detwinner::logic
