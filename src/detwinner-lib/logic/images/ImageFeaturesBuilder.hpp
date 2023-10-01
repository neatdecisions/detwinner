/*
 ===============================================================================
 Name        : ImageFeaturesBuilder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <atomic>
#include <string>
#include <vector>

#include <logic/callbacks/IImageFinderCallback.hpp>
#include <logic/images/ImageFeatures.hpp>

namespace detwinner::logic::images {

//==============================================================================
// ImageFeatureBuilder
//==============================================================================
class ImageFeaturesBuilder
{
public:
	ImageFeaturesBuilder(const std::vector<std::string> & fileNames,
	                     const callbacks::IImageFinderCallback::Ptr & callback);

	std::vector<ImageFeatures> execute();

private:
	std::vector<ImageFeatures> executeInternal(std::size_t startIndex, std::size_t endIndex);

	const std::vector<std::string> & m_fileNames;
	callbacks::IImageFinderCallback::Ptr m_callback;
	std::atomic_bool m_stopped;
	std::atomic_size_t m_processedCount;
};

} // namespace detwinner::logic::images
