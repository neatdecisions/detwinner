/*
 ===============================================================================
 Name        : ImageFeatures.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2022 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_IMAGES_IMAGEFEATURES_HPP_
#define LOGIC_IMAGES_IMAGEFEATURES_HPP_

#include <logic/images/HistogramT.hpp>


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// Histogram
//==============================================================================
using Histogram = HistogramT<32>;
using HistogramI = HistogramT<8>;


//==============================================================================
// ImageFeatures
//==============================================================================
struct ImageFeatures
{
	ImageFeatures(unsigned int id, float aspect);

	unsigned int getId() const noexcept { return id; }

	static constexpr uint8_t kSectionCount = 4;
	Histogram histY[kSectionCount];
	Histogram histU[kSectionCount];
	Histogram histV[kSectionCount];
	HistogramI histI[kSectionCount];

	float compare(const ImageFeatures & f, bool processRotations) const;

private:
	const unsigned int id;
	const float aspect;
};


}}}

#endif /* LOGIC_IMAGES_IMAGEFEATURES_HPP_ */
