/*
 ===============================================================================
 Name        : ImageFeatures.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/ImageFeatures.hpp>

#include <assert.h>
#include <cmath>
#include <limits>
#include <vector>
#include <numeric>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
ImageFeatures::ImageFeatures(const unsigned int id, const float aspect) :
	id(id), aspect(aspect)
{}


//------------------------------------------------------------------------------
template <class Histogram_t>
float
ImageFeatures::compareHistogram(const Histogram_t & h1, const Histogram_t & h2) const
{
	return h1.compare(h2);
}


//------------------------------------------------------------------------------
float
ImageFeatures::compare(const ImageFeatures & f, bool processRotations) const
{
	const bool isOrientationSame = ( (aspect > 1.0f) && (f.aspect > 1.0f) ) ||
	                               ( (aspect < 1.0f) && (f.aspect < 1.0f) );

	constexpr auto lambdaAvgCorrelation = [](const std::array<float, 4> & values) {
		constexpr std::array<float, 4> kFactors = { 1.0f, 1.0f, 1.0f, 1.0f };
		static const float kSum = std::accumulate(kFactors.begin(), kFactors.end(), 0.0f);
		return std::inner_product(values.begin(), values.end(), kFactors.begin(), 0.0f) / kSum;
	};

	std::array<float, 4> values{};
	float minVal = 2.0f;
	const size_t nRot = processRotations ? kSectionCount : 1;
	for (std::size_t i = 0; i < nRot; ++i)
	{
		std::fill(values.begin(), values.end(), 0.0f);
		for (auto sectionIndex = 0; sectionIndex < kSectionCount; ++sectionIndex)
		{
			const std::size_t sectionIndex2 = (i + sectionIndex) % kSectionCount;
			values[0] += compareHistogram(histY[sectionIndex], f.histY[sectionIndex2]);
			values[1] += compareHistogram(histU[sectionIndex], f.histU[sectionIndex2]);
			values[2] += compareHistogram(histV[sectionIndex], f.histV[sectionIndex2]);
			values[3] += compareHistogram(histI[sectionIndex], f.histI[sectionIndex2]);
		}

		std::transform(values.begin(), values.end(), values.begin(), [](float v) { return v / 4.0f; } );

		const float b = lambdaAvgCorrelation(values);

		if (b < minVal) minVal = b;
		if (!processRotations) break;
	}

	// TODO: in case of rotation, use the aspect depending on the rotation angle
	// avoid false positives on the images with completely different aspect ratio
	const float normalizedAspect2 = (!processRotations || isOrientationSame) ? f.aspect : 1.0f / f.aspect;

	const float aspectMultiplier = aspect > normalizedAspect2 ?
		aspect / normalizedAspect2 :
		normalizedAspect2 / aspect;

	return minVal * aspectMultiplier;
}


}}}
