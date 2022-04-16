/*
 ===============================================================================
 Name        : CommonDataTypes.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2022 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_COMMONDATATYPES_HPP_
#define LOGIC_COMMONDATATYPES_HPP_

#include <string>
#include <vector>
#include <optional>


namespace detwinner {
namespace logic {


struct DuplicateContainer
{
	struct FileDataInfo
	{
		struct ImageSize_t
		{
			unsigned int width = 0;
			unsigned int height = 0;
			ImageSize_t(unsigned int w, unsigned int h) : width(w), height(h){}
		};

		unsigned long long size;
		std::string name;
		std::optional<ImageSize_t> imageResolution;
		FileDataInfo(unsigned long long size, const std::string & fileName) :
			size(size), name(fileName) {}

		FileDataInfo(unsigned long long size, const std::string & fileName, unsigned int width, unsigned int height) :
			size(size), name(fileName), imageResolution(std::make_optional<ImageSize_t>(width, height)) {}
	};
	std::vector<FileDataInfo> files;
};


using DuplicatesList_t = std::vector<DuplicateContainer>;


}}

#endif /* COMMONDATATYPES_HPP_ */
