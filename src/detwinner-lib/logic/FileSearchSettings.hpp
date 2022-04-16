/*
 ===============================================================================
 Name        : FileSearchSettings.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2022 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_FILESEARCHSETTINGS_HPP_
#define LOGIC_FILESEARCHSETTINGS_HPP_

#include <optional>
#include <string>
#include <vector>


namespace detwinner {
namespace logic {


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


}}

#endif /* LOGIC_FILESEARCHSETTINGS_HPP_ */
