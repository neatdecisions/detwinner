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
			bool processRotations,
			const callbacks::IImageFinderCallback::Ptr_t & callback);
	SimilarityCache execute();

	using ParallelIndexes_t = std::vector< std::pair<std::size_t, std::size_t> >;
	static ParallelIndexes_t CalculateParallelIndexes(std::size_t count, std::size_t minBucketSize);

private:
	void calculateDistanceCache(std::size_t start, std::size_t end, SimilarityCache & cache);

	const std::vector<ImageFeatures> & m_imageFeatures;
	const bool m_processRotations;
	callbacks::IImageFinderCallback::Ptr_t m_callback;
	std::atomic_bool m_stopped;
	std::atomic_size_t m_processedCount;
};


}}}

#endif /* LOGIC_IMAGES_SIMILARITYCACHEBUILDER_HPP_ */
