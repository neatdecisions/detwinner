/*
 ===============================================================================
 Name        : SimilarImageFinder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_IMAGES_SIMILARIMAGEFINDER_HPP_
#define LOGIC_IMAGES_SIMILARIMAGEFINDER_HPP_


#include <set>
#include <vector>
#include <unordered_set>

#include <logic/callbacks/IImageFinderCallback.hpp>
#include <logic/images/DuplicateImageResult.hpp>
#include <logic/images/ImageFeatures.hpp>
#include <logic/images/Types.hpp>


namespace detwinner {
namespace logic {
namespace images {


class SimilarityCache;


//==============================================================================
// SimilarImageFinder
//==============================================================================
class SimilarImageFinder
{
public:
	DuplicateImageResult find(
		const std::vector<std::string> & fileNames,
		const unsigned short sensitivity,
		const bool processRotations,
		const callbacks::IImageFinderCallback::Ptr_t & callback) const;

private:
	using FeaturesVector = std::vector<ImageFeatures>;

	struct Cluster_t
	{
		Cluster_t() = default;
		Cluster_t(std::size_t id, std::size_t neighborId, Distance_t neighborDistance, std::size_t firstItemId) :
			id(id), neighborId(neighborId), neighborDistance(neighborDistance), items({firstItemId}) {}
		std::size_t id = 0;
		std::size_t neighborId = 0;
		Distance_t neighborDistance = 0;
		std::unordered_set<std::size_t> items;
	};

	void addClusterToResult(
			const Cluster_t & cluster,
			const std::vector<std::size_t> & imageIndexMap,
			const std::vector<std::string> & fileNames,
			DuplicateImageResult & result,
			const callbacks::IImageFinderCallback::Ptr_t & callback) const;

	std::size_t findMinimalDistanceIndex(
			const std::vector<Cluster_t> & clusters) const;

	std::size_t findIndexById(
			const std::vector<Cluster_t> & clusters,
			const std::size_t id,
			const std::size_t defaultValue) const;

	void updateDistanceCache(
			const std::vector<Cluster_t> & clusters,
			const Distance_t maxDistance,
			const std::size_t mergedClusterId,
			Cluster_t & newCluster,
			SimilarityCache & cache) const;

	void clusterize(
			const std::vector<std::size_t> & imageFeatureMap,
			const std::vector<std::string> & fileNames,
			const uint_least8_t sensitivity,
			SimilarityCache & cache,
			DuplicateImageResult & result,
			const callbacks::IImageFinderCallback::Ptr_t & callback) const;

	void updateNeighbours(
			const SimilarityCache & cache,
			const Distance_t maxDistance,
			const std::size_t mergedClusterId1,
			const std::size_t mergedClusterId2,
			std::vector<Cluster_t> & clusters,
			std::set<std::size_t> & outlierIndexes) const;

	std::set<std::size_t> updateNeighboursPartial(
			const SimilarityCache & cache,
			const Distance_t maxDistance,
			const std::size_t mergedClusterId1,
			const std::size_t mergedClusterId2,
			const std::size_t startIndex,
			const std::size_t endIndex,
			std::vector<Cluster_t> & clusters) const;

};


}}}

#endif /* LOGIC_IMAGES_SIMILARIMAGEFINDER_HPP_ */
