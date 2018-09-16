/*
 ===============================================================================
 Name        : DuplicateImageResult.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_IMAGES_DUPLICATEIMAGERESULT_HPP_
#define LOGIC_IMAGES_DUPLICATEIMAGERESULT_HPP_

#include <logic/images/ImageGroup.hpp>


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// DuplicateImageResult
//==============================================================================
class DuplicateImageResult
{
public:
	DuplicateImageResult() = default;
	DuplicateImageResult(const DuplicateImageResult&) = delete;
	DuplicateImageResult(DuplicateImageResult&&) = default;
	DuplicateImageResult & operator=(const DuplicateImageResult&) = delete;
	DuplicateImageResult & operator=(DuplicateImageResult&&) = default;
	void addImageGroup(ImageGroup && imageGroup);
	std::size_t getImageGroupCount() const;
	const ImageGroup & getImageGroup(const std::size_t i) const;

protected:
	std::vector<ImageGroup> m_imageInfos;
};


}}}

#endif /* LOGIC_IMAGES_DUPLICATEIMAGERESULT_HPP_ */
