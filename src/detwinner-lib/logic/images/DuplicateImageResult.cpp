/*
 ===============================================================================
 Name        : DuplicateImageResult.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/DuplicateImageResult.hpp>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
void
DuplicateImageResult::addImageGroup(ImageGroup && group)
{
	m_imageInfos.push_back(std::move(group));
}


//------------------------------------------------------------------------------
std::size_t
DuplicateImageResult::getImageGroupCount() const
{
	return m_imageInfos.size();
}


//------------------------------------------------------------------------------
const ImageGroup &
DuplicateImageResult::getImageGroup(const std::size_t i) const
{
	return m_imageInfos[i];
}


}}}

