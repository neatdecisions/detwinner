#ifndef TEST_LOGIC_IMAGES_IMAGEFEATURESTESTFACTORY_HPP_
#define TEST_LOGIC_IMAGES_IMAGEFEATURESTESTFACTORY_HPP_

#include <logic/images/ImageFeatures.hpp>


namespace detwinner {
namespace logic {
namespace images {


struct ImageFeaturesTestFactory
{
	static ImageFeatures CreateFeatures_1();
	static ImageFeatures CreateFeatures_2();
};


}}}

#endif
