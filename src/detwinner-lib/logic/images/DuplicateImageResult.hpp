/*
 ===============================================================================
 Name        : DuplicateImageResult.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <logic/images/ImageInfo.hpp>

namespace detwinner::logic::images {

//==============================================================================
// DuplicateImageResult
//==============================================================================
using ImageGroup = std::vector<ImageInfo>;
using DuplicateImageResult = std::vector<ImageGroup>;

} // namespace detwinner::logic::images
