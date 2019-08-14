/*
 ===============================================================================
 Name        : DuplicateImageResult.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_IMAGES_DUPLICATEIMAGERESULT_HPP_
#define LOGIC_IMAGES_DUPLICATEIMAGERESULT_HPP_

#include <logic/images/ImageInfo.hpp>


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// DuplicateImageResult
//==============================================================================
using ImageGroup = std::vector<ImageInfo>;
using DuplicateImageResult = std::vector<ImageGroup>;

}}}

#endif /* LOGIC_IMAGES_DUPLICATEIMAGERESULT_HPP_ */
