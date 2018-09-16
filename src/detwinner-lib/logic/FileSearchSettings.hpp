/*
 ===============================================================================
 Name        : FileSearchSettings.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_FILESEARCHSETTINGS_HPP_
#define LOGIC_FILESEARCHSETTINGS_HPP_

#include <compat/optional.hpp>
#include <string>
#include <vector>


namespace detwinner {
namespace logic {


struct FileSearchSettings
{
	stdx::optional<unsigned short> sensitivity;
	stdx::optional<bool> processRotations;
	stdx::optional<unsigned long long> minFileSize;
	stdx::optional<unsigned long long> maxFileSize;
	std::vector<std::string> filenameRegexps;
	bool searchReadOnlyFiles = true;
	bool searchHiddenFiles = false;
	bool searchExecutableFiles = false;
};


}}

#endif /* LOGIC_FILESEARCHSETTINGS_HPP_ */
