/*
 ===============================================================================
 Name        : CommonDataTypes.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_COMMONDATATYPES_HPP_
#define LOGIC_COMMONDATATYPES_HPP_

#include <map>
#include <memory>
#include <queue>
#include <vector>
#include <compat/optional.hpp>


namespace detwinner {
namespace logic {


struct DuplicateContainer
{
	struct FileDataInfo
	{
		struct ImageSize_t
		{
			unsigned int width;
			unsigned int height;
			ImageSize_t() : width(0), height(0){}
			ImageSize_t(unsigned int w, unsigned int h) : width(w), height(h){}
		};

		unsigned long long size;
		std::string name;
		stdx::optional<ImageSize_t> imageResolution;
		FileDataInfo(unsigned long long size, const std::string & fileName) :
			size(size), name(fileName) {}

		FileDataInfo(unsigned long long size, const std::string & fileName, unsigned int width, unsigned int height) :
			size(size), name(fileName), imageResolution(stdx::make_optional(ImageSize_t(width, height))) {}
	};
	DuplicateContainer() = default;
	explicit DuplicateContainer(const std::vector<FileDataInfo> & files) : files(files) {}
	std::vector<FileDataInfo> files;
};


using DuplicatesList_t = std::vector<DuplicateContainer>;


}}

#endif /* COMMONDATATYPES_HPP_ */
