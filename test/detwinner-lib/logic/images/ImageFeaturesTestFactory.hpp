#pragma once

#include <logic/images/ImageFeatures.hpp>

namespace detwinner::logic::images {

struct ImageFeaturesTestFactory
{
	static ImageFeatures CreateFeatures_1();
	static ImageFeatures CreateFeatures_2();
};

} // namespace detwinner::logic::images
