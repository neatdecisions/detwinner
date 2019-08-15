/*
 ===============================================================================
 Name        : ImageFeaturesBuilder.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/ImageFeaturesBuilder.hpp>

#include <logic/images/ImageFeaturesBridge.hpp>

#include <future>


namespace detwinner {
namespace logic {
namespace images {


constexpr unsigned int kMinimumFilesPerThread = 1000;


//------------------------------------------------------------------------------
ImageFeaturesBuilder::ImageFeaturesBuilder(
	const std::vector<std::string> & fileNames,
	const callbacks::IImageFinderCallback::Ptr_t & callback) :
	m_fileNames(fileNames), m_callback(callback),
	m_stopped(false), m_processedCount(0)
{}


//------------------------------------------------------------------------------
std::vector<ImageFeatures>
ImageFeaturesBuilder::execute()
{
	const std::size_t fns = m_fileNames.size();

	m_processedCount = 0;
	if (m_callback) m_callback->imgIndexingProgress(0, fns);
	// we don't want to create threads for a microtasks
	const unsigned int numThreads = (fns > kMinimumFilesPerThread) ?
		std::max(4U, std::thread::hardware_concurrency()) : 1;
	// distribute the list of filenames into separate tasks
	// which than will be run in a separate tasks
	const std::size_t delta = fns / numThreads;

	std::vector< std::future< std::vector<ImageFeatures> > > futures;

	const std::launch launchType = (numThreads > 1) ? std::launch::async : std::launch::deferred;

	for (unsigned int i = 0; i < numThreads; ++i)
	{
		const std::size_t start = i * delta;
		// the last bucket will go entirely to the last task
		const std::size_t end = ( i == (numThreads - 1) ) ? m_fileNames.size() : i * delta + delta;
		futures.push_back( std::async(
				launchType, &ImageFeaturesBuilder::executeInternal, this, start, end) );
	}

	std::vector<ImageFeatures> imageFeatures;
	imageFeatures.reserve(fns);
	for (auto && future: futures)
	{
		auto partialResult = future.get();

		if (imageFeatures.empty())
		{
			imageFeatures = std::move(partialResult);
		} else
		{
			imageFeatures.reserve(imageFeatures.size() + partialResult.size());
			std::move(partialResult.begin(), partialResult.end(), std::back_inserter(imageFeatures));
		}
	}

	return imageFeatures;
}


//------------------------------------------------------------------------------
std::vector<ImageFeatures>
ImageFeaturesBuilder::executeInternal(const std::size_t startIndex, const std::size_t endIndex)
{
	static const Magick::Geometry kResizeGeometry(128, 128); // higher
	std::vector<ImageFeatures> imageFeatures;
	if (endIndex > startIndex)
	{
		imageFeatures.reserve(endIndex - startIndex);
	}
	Magick::Image img;
	img.quiet(true); // avoid warning exceptions
	for (std::size_t i = startIndex; (i < endIndex) && !m_stopped; ++i)
	{
		try
		{
			img.read(kResizeGeometry, m_fileNames.at(i));
			imageFeatures.push_back(ImageFeaturesBridge::GetImageFeatures(img, i));
		} catch (const Magick::Exception&)
		{
			// normal to have it here if the file being read is not an image
		}
		if (m_callback)
		{
			++m_processedCount;
			m_callback->imgIndexingProgress(m_processedCount, m_fileNames.size());
			if (m_callback->pauseAndStopStatus()) m_stopped = true;
		}
	}
	return imageFeatures;
}


}}}
