/*
 ===============================================================================
 Name        : ImageFeaturesBridge.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <Magick++.h>

#include <logic/images/ImageFeatures.hpp>

namespace detwinner::logic::images {

//==============================================================================
// ImageFeaturesBridge
//==============================================================================
class ImageFeaturesBridge
{
public:
	static ImageFeatures GetImageFeatures(Magick::Image & image, unsigned int id);

	ImageFeaturesBridge() = delete;
	~ImageFeaturesBridge() = delete;

private:
	static void GetIntensityHistogram(const Magick::Image & image, const Magick::Geometry & roi, HistogramI & histI);

	static void GetYUVHistograms(const Magick::Image & image,
	                             const Magick::Geometry & roi,
	                             Histogram & histY,
	                             Histogram & histU,
	                             Histogram & histV);
};

} // namespace detwinner::logic::images