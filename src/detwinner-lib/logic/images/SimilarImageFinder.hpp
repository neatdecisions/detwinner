/*
 ===============================================================================
 Name        : SimilarImageFinder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <set>
#include <unordered_set>
#include <vector>

#include <logic/callbacks/IImageFinderCallback.hpp>
#include <logic/images/DuplicateImageResult.hpp>
#include <logic/images/ImageFeatures.hpp>
#include <logic/images/Types.hpp>

namespace detwinner::logic::images {

class SimilarityCache;

//==============================================================================
// SimilarImageFinder
//==============================================================================
class SimilarImageFinder
{
public:
	DuplicateImageResult find(const std::vector<std::string> & fileNames,
	                          unsigned short sensitivity,
	                          bool processRotations,
	                          const callbacks::IImageFinderCallback::Ptr & callback) const;

private:
	using FeaturesVector = std::vector<ImageFeatures>;

	struct Cluster
	{
		Cluster() = default;
		Cluster(std::size_t id, std::size_t neighborId, Distance_t neighborDistance, std::size_t firstItemId)
				: id(id), neighborId(neighborId), neighborDistance(neighborDistance), items({firstItemId})
		{
		}
		std::size_t id = 0;
		std::size_t neighborId = 0;
		Distance_t neighborDistance = 0;
		std::unordered_set<std::size_t> items;
	};

	void addClusterToResult(const Cluster & cluster,
	                        const std::vector<std::size_t> & imageIndexMap,
	                        const std::vector<std::string> & fileNames,
	                        DuplicateImageResult & result,
	                        const callbacks::IImageFinderCallback::Ptr & callback) const;

	std::size_t findMinimalDistanceIndex(const std::vector<Cluster> & clusters) const;

	std::size_t findIndexById(const std::vector<Cluster> & clusters, std::size_t id, std::size_t defaultValue) const;

	void updateDistanceCache(const std::vector<Cluster> & clusters,
	                         Distance_t maxDistance,
	                         std::size_t mergedClusterId,
	                         Cluster & newCluster,
	                         SimilarityCache & cache) const;

	void clusterize(const std::vector<std::size_t> & imageFeatureMap,
	                const std::vector<std::string> & fileNames,
	                uint_least8_t sensitivity,
	                SimilarityCache & cache,
	                DuplicateImageResult & result,
	                const callbacks::IImageFinderCallback::Ptr & callback) const;

	void updateNeighbours(const SimilarityCache & cache,
	                      Distance_t maxDistance,
	                      std::size_t mergedClusterId1,
	                      std::size_t mergedClusterId2,
	                      std::vector<Cluster> & clusters,
	                      std::set<std::size_t> & outlierIndexes) const;

	std::set<std::size_t> updateNeighboursPartial(const SimilarityCache & cache,
	                                              Distance_t maxDistance,
	                                              std::size_t mergedClusterId1,
	                                              std::size_t mergedClusterId2,
	                                              std::size_t startIndex,
	                                              std::size_t endIndex,
	                                              std::vector<Cluster> & clusters) const;
};

} // namespace detwinner::logic::images
