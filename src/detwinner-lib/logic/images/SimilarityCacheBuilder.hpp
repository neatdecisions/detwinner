/*
 ============================================================================
 Name        : SimilarityCacheBuilder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */

#ifndef LOGIC_IMAGES_SIMILARITYCACHEBUILDER_HPP_
#define LOGIC_IMAGES_SIMILARITYCACHEBUILDER_HPP_


#include <atomic>
#include <vector>
#include <unordered_set>
#include <logic/callbacks/IImageFinderCallback.hpp>
#include <logic/images/ImageFeatures.hpp>
#include <logic/images/SimilarityCache.hpp>
#include <logic/images/Types.hpp>

namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// SimilarityCacheBuilder
//==============================================================================
class SimilarityCacheBuilder
{
public:
	SimilarityCacheBuilder(
			const std::vector<ImageFeatures> & imageFeatures,
			const bool processRotations,
			const callbacks::IImageFinderCallback::Ptr_t & callback);
	SimilarityCache execute();

	using ParallelIndexes_t = std::vector< std::pair<std::size_t, std::size_t> >;
	static ParallelIndexes_t CalculateParallelIndexes(const std::size_t count, const std::size_t minBucketSize);

private:
	void calculateDistanceCache(const std::size_t start, const std::size_t end, SimilarityCache & cache);

	const std::vector<ImageFeatures> & m_imageFeatures;
	const bool m_processRotations;
	callbacks::IImageFinderCallback::Ptr_t m_callback;
	std::atomic<bool> m_stopped;
	std::atomic<std::size_t> m_processedCount;
};


}}}

#endif /* LOGIC_IMAGES_SIMILARITYCACHEBUILDER_HPP_ */
