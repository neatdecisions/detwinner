/*
 ===============================================================================
 Name        : SearchSettings.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef SETTINGS_SEARCHSETTINGS_HPP_
#define SETTINGS_SEARCHSETTINGS_HPP_

#include <optional>
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

	struct CommonSettings
	{
		std::optional<FileSizeSetting_t> minFileSize;
		std::optional<FileSizeSetting_t> maxFileSize;
		std::vector<std::string> filenameRegexps;
		bool searchReadOnly = true;
		bool searchHidden = false;
		bool searchExecutable = true;
	};

	struct UiSettings
	{
		bool showHiddenFiles = false;
	};

	CommonSettings exactDuplicatesSettings;
	CommonSettings similarImagesSettings;

	SearchMode_t searchMode = SearchMode_t::kExactDuplicates;
	ImageSettings_t imageSettings;

	UiSettings uiSettings;
};



}}

#endif /* SETTINGS_SEARCHSETTINGS_HPP_ */
