/*
 ===============================================================================
 Name        : CommonDataTypes.hpp
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

struct DuplicateContainer
{
	struct FileDataInfo
	{
		struct ImageSize
		{
			unsigned int width = 0;
			unsigned int height = 0;
			ImageSize(unsigned int w, unsigned int h) : width(w), height(h) {}
		};

		unsigned long long size;
		std::string name;
		std::optional<ImageSize> imageResolution;
		FileDataInfo(unsigned long long size, const std::string & fileName) : size(size), name(fileName) {}

		FileDataInfo(unsigned long long size, const std::string & fileName, unsigned int width, unsigned int height)
				: size(size), name(fileName), imageResolution(std::make_optional<ImageSize>(width, height))
		{
		}
	};
	std::vector<FileDataInfo> files;
};

using DuplicatesList = std::vector<DuplicateContainer>;

} // namespace detwinner::logic
