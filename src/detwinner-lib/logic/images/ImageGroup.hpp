/*
 ============================================================================
 Name        : ImageGroup.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */

#ifndef LOGIC_IMAGES_IMAGEGROUP_HPP_
#define LOGIC_IMAGES_IMAGEGROUP_HPP_

#include <map>
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


//===========================================================================
// ImageGroup
//===========================================================================
class ImageGroup
{
public:
	explicit ImageGroup(const std::size_t sizeToReserve);
	void addImageInfo(
		const std::string & fileName,
		const unsigned long long fileSize,
		const unsigned int width,
		const unsigned int height);
	std::size_t getImageCount() const;
	const ImageInfo & getImageInfo(const std::size_t i) const;
	ImageGroup() = default;
	ImageGroup(const ImageGroup&) = delete;
	ImageGroup(ImageGroup&&) = default;
	ImageGroup & operator=(const ImageGroup&) = delete;
	ImageGroup & operator=(ImageGroup&&) = default;
	~ImageGroup() noexcept = default;

protected:
	std::vector<ImageInfo> m_imageInfos;
};


}}}

#endif /* IMAGEGROUP_HPP_ */
