/*
 ===============================================================================
 Name        : ImageFeaturesBuilder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2022 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_IMAGES_IMAGEFEATURESBUILDER_HP_
#define LOGIC_IMAGES_IMAGEFEATURESBUILDER_HP_


#include <atomic>
#include <string>
#include <vector>

#include <logic/callbacks/IImageFinderCallback.hpp>
#include <logic/images/ImageFeatures.hpp>


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// ImageFeatureBuilder
//==============================================================================
class ImageFeaturesBuilder
{
public:
	ImageFeaturesBuilder(
		const std::vector<std::string> & fileNames,
		const callbacks::IImageFinderCallback::Ptr_t & callback);

	std::vector<ImageFeatures> execute();

private:
	std::vector<ImageFeatures> executeInternal(
		std::size_t startIndex,
		std::size_t endIndex);

	const std::vector<std::string> & m_fileNames;
	callbacks::IImageFinderCallback::Ptr_t m_callback;
	std::atomic_bool m_stopped;
	std::atomic_size_t m_processedCount;
};


}}}

#endif /* LOGIC_IMAGES_IMAGEFEATURESBUILDER_HPP_ */
