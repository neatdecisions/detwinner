/*
 ============================================================================
 Name        : SimilarityCache.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2022 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */

#ifndef LOGIC_IMAGES_SIMILARITYCACHE_HPP_
#define LOGIC_IMAGES_SIMILARITYCACHE_HPP_

#include <vector>

#include <logic/images/Types.hpp>


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// SimilarityCache
//==============================================================================
class SimilarityCache
{
public:
	explicit SimilarityCache(std::size_t size);
	SimilarityCache(const SimilarityCache&) = delete;
	SimilarityCache& operator=(const SimilarityCache&) = delete;
	SimilarityCache(SimilarityCache&&) = default;
	SimilarityCache& operator=(SimilarityCache&&) = default;
	~SimilarityCache() noexcept = default;
	void set(std::size_t i1, std::size_t i2, Distance_t val) noexcept;
	Distance_t get(std::size_t i1, std::size_t i2) const noexcept;
protected:
	const std::size_t m_size;
	std::vector<Distance_t> m_matrix;
};


}}}

#endif /* LOGIC_IMAGES_SIMILARITYCACHE_HPP_ */
