/*
 ============================================================================
 Name        : ImageInfo.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */

#pragma once

#include <string>
#include <vector>

namespace detwinner::logic::images {

//===========================================================================
// ImageInfo
//===========================================================================
struct ImageInfo
{
	std::string fileName;
	unsigned long long fileSize = 0;
	unsigned int width = 0;
	unsigned int height = 0;

	ImageInfo(const std::string & fn, unsigned long long fs, unsigned int w, unsigned int h)
			: fileName(fn), fileSize(fs), width(w), height(h)
	{
	}
};

} // namespace detwinner::logic::images
