/*
 ===============================================================================
 Name        : SearchSettings.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef SETTINGS_SEARCHSETTINGS_HPP_
#define SETTINGS_SEARCHSETTINGS_HPP_

#include <compat/optional.hpp>
#include <string>
#include <vector>


namespace detwinner {
namespace settings {


struct SearchSettings
{
	enum class SearchMode_t
	{
		kExactDuplicates,
		kSimilarImages
	};

	enum class FileSizeUnit_t
	{
		kB = 0,
		kKB = 1,
		kMB = 2,
		kGB = 3
	};

	struct FileSizeSetting_t
	{
		bool enabled = false;
		unsigned long long size = 0;
		FileSizeUnit_t unit = FileSizeUnit_t::kB;
	};

	struct ImageSettings_t
	{
		unsigned short sensitivity = 85;
		bool processRotations = true;
	};

	SearchMode_t searchMode = SearchMode_t::kExactDuplicates;
	stdx::optional<ImageSettings_t> imageSettings;
	stdx::optional<FileSizeSetting_t> minFileSize;
	stdx::optional<FileSizeSetting_t> maxFileSize;
	std::vector<std::string> filenameRegexps;
	bool searchReadOnly = true;
	bool searchHidden = false;
	bool searchExecutable = true;
};



}}

#endif /* SETTINGS_SEARCHSETTINGS_HPP_ */
