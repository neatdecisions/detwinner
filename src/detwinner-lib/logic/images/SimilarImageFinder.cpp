/*
 ===============================================================================
 Name        : SimilarImageFinder.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/SimilarImageFinder.hpp>

#include <assert.h>
#include <future>
#include <vector>

#include <Magick++.h>

#include <logic/images/ImageFeaturesBuilder.hpp>
#include <logic/images/SimilarityCacheBuilder.hpp>

namespace detwinner::logic::images {

//------------------------------------------------------------------------------
template <typename T>
void
deleteFromVectorBySwap(std::vector<T> & vec, std::size_t i) noexcept
{
	const std::size_t n = vec.size();
	if (n == 0) return;
	if (i >= n) return;
	if (i < (n - 1))
	{
		std::swap(vec[i], vec.back());
	}
	vec.resize(n - 1);
}

//==============================================================================
// SimilarImageFinder
//==============================================================================

//------------------------------------------------------------------------------
std::set<std::size_t>
SimilarImageFinder::updateNeighboursPartial(const SimilarityCache & cache,
                                            Distance_t maxDistance,
                                            std::size_t mergedClusterId1,
                                            std::size_t mergedClusterId2,
                                            std::size_t startIndex,
                                            std::size_t endIndex,
                                            std::vector<Cluster> & clusters) const
{
	std::set<std::size_t> outliers;
	for (std::size_t i = startIndex; i < endIndex; ++i)
	{
		Cluster & cluster = clusters[i];
		if (cluster.neighborDistance > maxDistance) continue;

		if (cluster.id == mergedClusterId1) continue;
		if ((cluster.neighborId == mergedClusterId1) || (cluster.neighborId == mergedClusterId2))
		{
			cluster.neighborId = mergedClusterId1;
			cluster.neighborDistance = cache.get(cluster.id, mergedClusterId1);
			for (const auto & subcluster : clusters)
			{
				if ((subcluster.id == mergedClusterId1) || (subcluster.id == cluster.id)) continue;
				const Distance_t dist = cache.get(cluster.id, subcluster.id);
				if (dist < cluster.neighborDistance)
				{
					cluster.neighborDistance = dist;
					cluster.neighborId = subcluster.id;
				}
			}
			if (cluster.neighborDistance > maxDistance)
			{
				outliers.insert(i);
			}
		}
	}
	return outliers;
}

//------------------------------------------------------------------------------
void
SimilarImageFinder::updateNeighbours(const SimilarityCache & cache,
                                     Distance_t maxDistance,
                                     std::size_t mergedClusterId1,
                                     std::size_t mergedClusterId2,
                                     std::vector<Cluster> & clusters,
                                     std::set<std::size_t> & outlierIndexes) const
{
	const SimilarityCacheBuilder::ParallelIndexes_t parallelIndexes =
			SimilarityCacheBuilder::CalculateParallelIndexes(clusters.size(), 1000);

	std::vector<std::future<std::set<std::size_t>>> futures;
	if (parallelIndexes.empty())
	{
		futures.push_back(std::async(std::launch::deferred, &SimilarImageFinder::updateNeighboursPartial, this,
		                             std::ref(cache), maxDistance, mergedClusterId1, mergedClusterId2, 0, clusters.size(),
		                             std::ref(clusters)));
	} else
	{
		for (const auto & indexPair : parallelIndexes)
		{
			futures.push_back(std::async(std::launch::async, &SimilarImageFinder::updateNeighboursPartial, this,
			                             std::ref(cache), maxDistance, mergedClusterId1, mergedClusterId2, indexPair.first,
			                             indexPair.second, std::ref(clusters)));
		}
	}

	for (auto && future : futures)
	{
		const std::set<std::size_t> & set1 = future.get();
		outlierIndexes.insert(set1.begin(), set1.end());
	}
}

//------------------------------------------------------------------------------
void
SimilarImageFinder::addClusterToResult(const Cluster & cluster,
                                       const std::vector<std::size_t> & imageIndexMap,
                                       const std::vector<std::string> & fileNames,
                                       DuplicateImageResult & result,
                                       const callbacks::IImageFinderCallback::Ptr & callback) const
{
	if (cluster.items.size() > 1)
	{
		Magick::Image pingImage;
		pingImage.quiet(true); // avoid warning exceptions
		const std::size_t similaritiesSize = imageIndexMap.size();
		ImageGroup imgGroup;
		imgGroup.reserve(cluster.items.size());
		unsigned long long totalFilesSize = 0ULL;
		unsigned long long maxFileSize = 0ULL;
		for (auto && featureIndex : cluster.items)
		{
			if (featureIndex < similaritiesSize)
			{
				try
				{
					const std::string & fileName = fileNames.at(imageIndexMap[featureIndex]);
					pingImage.ping(fileName);
					const unsigned long long fileSize = pingImage.fileSize();
					totalFilesSize += fileSize;
					if (fileSize > maxFileSize) maxFileSize = fileSize;
					imgGroup.emplace_back(fileName, fileSize, pingImage.columns(), pingImage.rows());
				} catch (...)
				{
					continue;
				}
			} else
			{
				assert(false);
			}
		}
		if (imgGroup.size() > 1)
		{
			if (callback) callback->similarImagesFound(imgGroup.size(), totalFilesSize, totalFilesSize - maxFileSize);
			result.push_back(std::move(imgGroup));
		}
	}
}

//------------------------------------------------------------------------------
std::size_t
SimilarImageFinder::findMinimalDistanceIndex(const std::vector<Cluster> & clusters) const
{
	auto it = std::min_element(clusters.begin(), clusters.end(), [](const Cluster & c1, const Cluster & c2) {
		return c1.neighborDistance < c2.neighborDistance;
	});
	return std::distance(clusters.begin(), it);
}

//------------------------------------------------------------------------------
void
SimilarImageFinder::updateDistanceCache(const std::vector<Cluster> & clusters,
                                        Distance_t maxDistance,
                                        std::size_t mergedClusterId,
                                        Cluster & newCluster,
                                        SimilarityCache & cache) const
{
	newCluster.neighborDistance = maxDistance + 1;
	newCluster.neighborId = newCluster.id;

	for (auto && cluster : clusters)
	{
		assert(cluster.id != mergedClusterId);
		if (cluster.neighborDistance > maxDistance) continue;
		if (cluster.id == newCluster.id) continue;

		const Distance_t dist = std::max(cache.get(newCluster.id, cluster.id), cache.get(mergedClusterId, cluster.id));
		cache.set(newCluster.id, cluster.id, dist);
		if (dist < newCluster.neighborDistance)
		{
			newCluster.neighborDistance = dist;
			newCluster.neighborId = cluster.id;
		}
	}
}

//------------------------------------------------------------------------------
std::size_t
SimilarImageFinder::findIndexById(const std::vector<Cluster> & clusters, std::size_t id, std::size_t defaultValue) const
{
	auto it = std::find_if(clusters.begin(), clusters.end(), [&id](const Cluster & c) { return c.id == id; });
	return (it == clusters.end()) ? defaultValue : std::distance(clusters.begin(), it);
}

//------------------------------------------------------------------------------
void
SimilarImageFinder::clusterize(const std::vector<std::size_t> & imageIndexMap,
                               const std::vector<std::string> & fileNames,
                               uint_least8_t sensitivity,
                               SimilarityCache & cache,
                               DuplicateImageResult & result,
                               const callbacks::IImageFinderCallback::Ptr & callback) const
{
	const std::size_t count = imageIndexMap.size();

	const Distance_t d = (sensitivity > 100) ? 0 : 100 - sensitivity;
	std::size_t progress = 0;

	std::vector<Cluster> clusters;
	clusters.reserve(count);

	if (callback) callback->imgOrganizingProgress(0, count);

	// remove outliers
	for (std::size_t i = 0; i < count; ++i)
	{
		Distance_t minDistance = d;
		std::size_t neighborId = i;
		bool isOutlier = true;
		for (std::size_t j = 0; j < count; ++j)
		{
			if (i == j) continue;
			const Distance_t distance = cache.get(i, j);
			if (distance <= minDistance)
			{
				minDistance = distance;
				neighborId = j;
				isOutlier = false;
			}
		}

		if (isOutlier)
		{
			if (callback) callback->imgOrganizingProgress(++progress, count);
		} else
		{
			clusters.emplace_back(i, neighborId, minDistance, i);
		}
	}

	while (clusters.size() > 1)
	{
		// find a cluster with the minimal distance to its closest neighbour
		std::size_t clusterPos1 = findMinimalDistanceIndex(clusters);

		assert(clusterPos1 < clusters.size());

		const std::size_t mergedClusterId = clusters[clusterPos1].neighborId;
		if (clusters[clusterPos1].neighborDistance > d || (clusters[clusterPos1].id == mergedClusterId)) break;
		std::size_t clusterPos2 = findIndexById(clusters, mergedClusterId, clusterPos1);

		assert(clusterPos1 != clusterPos2);

		// clusterPos1 will hold the merged cluster, clusterPos2 will be deleted
		if (clusterPos2 < clusterPos1) std::swap(clusterPos1, clusterPos2);
		Cluster & newCluster = clusters[clusterPos1];
		newCluster.items.insert(clusters[clusterPos2].items.begin(), clusters[clusterPos2].items.end());
		deleteFromVectorBySwap(clusters, clusterPos2);

		if (callback) callback->imgOrganizingProgress(++progress, count);

		// update cache for all distances involving the new cluster
		updateDistanceCache(clusters, d, mergedClusterId, newCluster, cache);

		// if a new distance to the closest neighbor is greater than requested, mark new cluster as an outlier
		std::set<std::size_t> outliers;
		if (newCluster.neighborDistance > d)
		{
			outliers.insert(clusterPos1);
		}

		// update all clusters which had as the closest neighbour any of the merged clusters
		updateNeighbours(cache, d, newCluster.id, mergedClusterId, clusters, outliers);

		// process newly formed outliers: add to result or just drop
		for (auto && it = outliers.rbegin(); it != outliers.rend(); ++it)
		{
			addClusterToResult(clusters[*it], imageIndexMap, fileNames, result, callback);
			deleteFromVectorBySwap(clusters, *it);
		}

		if (callback && !outliers.empty())
		{
			progress += outliers.size();
			callback->imgOrganizingProgress(progress, count);
		}
	}

	// in case if there are any valid clusters remaining, add them to the result
	for (auto && cluster : clusters)
	{
		addClusterToResult(cluster, imageIndexMap, fileNames, result, callback);
		if (callback)
		{
			callback->imgOrganizingProgress(++progress, count);
			if (callback->pauseAndStopStatus()) return;
		}
	}
}

//------------------------------------------------------------------------------
DuplicateImageResult
SimilarImageFinder::find(const std::vector<std::string> & fileNames,
                         unsigned short sensitivity,
                         bool processRotations,
                         const callbacks::IImageFinderCallback::Ptr & callback) const
{
	std::vector<ImageFeatures> imageFeatures = ImageFeaturesBuilder(fileNames, callback).execute();

	// similar image clusters
	DuplicateImageResult res;
	if (imageFeatures.empty() || (callback && callback->pauseAndStopStatus()))
	{
		return res;
	}

	SimilarityCache cache = SimilarityCacheBuilder(imageFeatures, processRotations, callback).execute();

	std::vector<std::size_t> imageFeatureMap(imageFeatures.size());
	std::transform(imageFeatures.begin(), imageFeatures.end(), imageFeatureMap.begin(),
	               [](const ImageFeatures & f) { return f.getId(); });
	// release some memory
	std::vector<ImageFeatures>().swap(imageFeatures);

	// perform hierarchical cluster analysis on the image fingerprints
	clusterize(imageFeatureMap, fileNames, sensitivity, cache, res, callback);
	return res;
}

} // namespace detwinner::logic::images
