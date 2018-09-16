/*
 ===============================================================================
 Name        : ImageGroup.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/ImageGroup.hpp>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
ImageGroup::ImageGroup(const std::size_t sizeToReserve)
{
	m_imageInfos.reserve(sizeToReserve);
}


//------------------------------------------------------------------------------
void
ImageGroup::addImageInfo(
	const std::string & fileName, const unsigned long long fileSize,
	const unsigned int width, const unsigned int height)
{
	m_imageInfos.emplace_back(fileName, fileSize, width, height);
}


//------------------------------------------------------------------------------
std::size_t
ImageGroup::getImageCount() const
{
	return m_imageInfos.size();
}


//------------------------------------------------------------------------------
const ImageInfo &
ImageGroup::getImageInfo(const std::size_t i) const
{
	return m_imageInfos[i];
}


}}}
