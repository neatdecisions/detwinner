/*
 ===============================================================================
 Name        : SearchSettings.hpp
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

namespace detwinner::settings {

struct SearchSettings
{
	enum class SearchMode : int { ExactDuplicates = 0, SimilarImages = 1 };

	enum class FileSizeUnit { B = 0, KB = 1, MB = 2, GB = 3 };

	struct FileSizeSetting
	{
		bool enabled = false;
		unsigned long long size = 0;
		FileSizeUnit unit = FileSizeUnit::B;
	};

	struct ImageSettings
	{
		unsigned short sensitivity = 85;
		bool processRotations = true;
	};

	struct CommonSettings
	{
		std::optional<FileSizeSetting> minFileSize;
		std::optional<FileSizeSetting> maxFileSize;
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

	SearchMode searchMode = SearchMode::ExactDuplicates;
	ImageSettings imageSettings;

	UiSettings uiSettings;
};

} // namespace detwinner::settings
