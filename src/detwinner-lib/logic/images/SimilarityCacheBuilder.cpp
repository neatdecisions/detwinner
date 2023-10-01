/*
 ===============================================================================
 Name        : SimilarityCacheBuilder.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/SimilarityCacheBuilder.hpp>

#include <algorithm>
#include <future>

namespace detwinner::logic::images {

//------------------------------------------------------------------------------
SimilarityCacheBuilder::SimilarityCacheBuilder(const std::vector<ImageFeatures> & imageFeatures,
                                               bool processRotations,
                                               const callbacks::IImageFinderCallback::Ptr & callback)
		: m_imageFeatures(imageFeatures), m_processRotations(processRotations), m_callback(callback), m_stopped(false),
			m_processedCount(0)
{
}

//------------------------------------------------------------------------------
SimilarityCacheBuilder::ParallelIndexes_t
SimilarityCacheBuilder::CalculateParallelIndexes(std::size_t count, std::size_t minBucketSize)
{
	ParallelIndexes_t result;

	const std::size_t totalCount = (count - 1) * count / 2;
	const unsigned int maxThreadCount = std::max(4U, std::thread::hardware_concurrency());
	const unsigned int bucketSize = totalCount / maxThreadCount;

	if (bucketSize > minBucketSize)
	{
		unsigned int threadCount = 1;
		std::size_t currentBucketSize = 0;
		std::size_t start = 0;
		for (std::size_t i = 0; i < count; ++i)
		{
			currentBucketSize += (count - i - 1);
			if (currentBucketSize >= bucketSize)
			{
				currentBucketSize = 0;
				result.emplace_back(start, i);

				start = i;
				++threadCount;
				if (threadCount >= maxThreadCount)
				{
					result.emplace_back(start, count);
					break;
				}
			}
		}
	}

	return result;
}

//------------------------------------------------------------------------------
SimilarityCache
SimilarityCacheBuilder::execute()
{
	const std::size_t count = m_imageFeatures.size();

	m_processedCount = 0;
	if (m_callback) m_callback->imgComparingProgress(0, count);

	SimilarityCache cache(count);
	ParallelIndexes_t parallelIndexes = CalculateParallelIndexes(count, 1000);
	std::vector<std::future<void>> futures;
	if (parallelIndexes.empty())
	{
		futures.push_back(std::async(std::launch::deferred, &SimilarityCacheBuilder::calculateDistanceCache, this, 0, count,
		                             std::ref(cache)));
	} else
	{
		for (auto & [indexStart, indexEnd] : parallelIndexes)
		{
			futures.push_back(std::async(std::launch::async, &SimilarityCacheBuilder::calculateDistanceCache, this,
			                             indexStart, indexEnd, std::ref(cache)));
		}
	}
	std::for_each(futures.begin(), futures.end(), [](auto & future) { future.wait(); });
	futures.clear();

	if (m_callback) m_callback->imgComparingProgress(m_imageFeatures.size(), m_imageFeatures.size());

	return cache;
}

//------------------------------------------------------------------------------
void
SimilarityCacheBuilder::calculateDistanceCache(std::size_t start, std::size_t end, SimilarityCache & cache)
{
	if (start >= end) return;
	const std::size_t sz = m_imageFeatures.size();
	for (std::size_t i = start; i < end - 1; ++i)
	{
		for (std::size_t j = i + 1; j < sz; ++j)
		{
			const int val = 100.0f * m_imageFeatures[i].compare(m_imageFeatures[j], m_processRotations);
			cache.set(i, j, std::min(val, DISTANCE_T_MAX));
		}

		if (m_callback)
		{
			++m_processedCount;
			m_callback->imgComparingProgress(m_processedCount, sz);
			if (m_callback->pauseAndStopStatus()) m_stopped = true;
			if (m_stopped) return;
		}
	}
}

} // namespace detwinner::logic::images
