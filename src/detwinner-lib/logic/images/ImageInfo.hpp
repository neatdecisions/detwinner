/*
 ============================================================================
 Name        : ImageInfo.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */

#ifndef LOGIC_IMAGES_IMAGEINFO_HPP_
#define LOGIC_IMAGES_IMAGEINFO_HPP_

#include <string>
#include <vector>


namespace detwinner {
namespace logic {
namespace images {


//===========================================================================
// ImageInfo
//===========================================================================
struct ImageInfo
{
	std::string fileName;
	unsigned long long fileSize = 0;
	unsigned int width = 0;
	unsigned int height = 0;

	ImageInfo(const std::string & fn, unsigned long long fs, unsigned int w, unsigned int h) :
		fileName(fn), fileSize(fs), width(w), height(h)
	{}
};


}}}

#endif /* LOGIC_IMAGES_IMAGEINFO_HPP_ */
