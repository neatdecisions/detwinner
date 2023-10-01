/*
 ===============================================================================
 Name        : SimilarityCache.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <algorithm>

#include <logic/images/SimilarityCache.hpp>

namespace detwinner::logic::images {

//------------------------------------------------------------------------------
SimilarityCache::SimilarityCache(std::size_t size)
		: m_size(size), m_matrix(size > 0 ? ((size - 1) * (1 + (size - 1))) / 2 : 1, DISTANCE_T_MAX)
{
}

//------------------------------------------------------------------------------
void
SimilarityCache::set(std::size_t i1, std::size_t i2, Distance_t val) noexcept
{
	if (i1 >= m_size || i2 >= m_size) return;
	if (i1 == i2) return;

	const auto mm = std::minmax(i1, i2);
	const size_t n = (mm.first * m_size - (mm.first * (mm.first + 1)) / 2) + mm.second - mm.first - 1;
	m_matrix[n] = val;
}

//------------------------------------------------------------------------------
Distance_t
SimilarityCache::get(std::size_t i1, std::size_t i2) const noexcept
{
	if (i1 >= m_size || i2 >= m_size) return DISTANCE_T_MAX;
	if (i1 == i2) return 0U;

	const auto mm = std::minmax(i1, i2);
	const size_t n = (mm.first * m_size - (mm.first * (mm.first + 1)) / 2) + mm.second - mm.first - 1;
	return m_matrix[n];
}

} // namespace detwinner::logic::images
