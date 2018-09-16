/*
 ===============================================================================
 Name        : ImageFeatures.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
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
	ImageFeatures(const unsigned int id, const float aspect);

	unsigned int getId() const { return id; }

	Histogram histY[4];
	Histogram histU[4];
	Histogram histV[4];
	HistogramI histI[4];

	float compare(const ImageFeatures & f, bool processRotations) const;

private:
	template <class Histogram_t>
	float compareHistogram(const Histogram_t & h1, const Histogram_t & h2) const;

	unsigned int id;
	float aspect;
};


}}}

#endif /* LOGIC_IMAGES_IMAGEFEATURES_HPP_ */
