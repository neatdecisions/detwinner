/*
 ===============================================================================
 Name        : SimilarityCache.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/SimilarityCache.hpp>

#include <assert.h>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
SimilarityCache::SimilarityCache(const std::size_t size) :
	m_size(size),
	m_matrix(size > 0 ? ((size - 1) * (1 + (size - 1))) / 2 : 1, -1)
{}


//------------------------------------------------------------------------------
void
SimilarityCache::set(std::size_t i1, std::size_t i2, Distance_t val)
{
	if (i1 >= m_size || i2 >= m_size)
	{
		assert(false);
		return;
	}

	if (i1 == i2)
	{
		assert(false);
		return;
	}

	if (i1 > i2)
	{
		std::swap(i1, i2);
	}

	const size_t n = (i1 * m_size - (i1 * (i1 + 1)) / 2) + i2 - i1 - 1;

	m_matrix[n] = val;
}


//------------------------------------------------------------------------------
Distance_t
SimilarityCache::get(std::size_t i1, std::size_t i2) const
{
	if (i1 >= m_size || i2 >= m_size)
	{
		return 100U;
	}

	if (i1 == i2)
	{
		return 0U;
	}

	if (i1 > i2)
	{
		std::swap(i1, i2);
	}

	const size_t n = (i1 * m_size - (i1 * (i1 + 1)) / 2) + i2 - i1 - 1;

	return m_matrix[n];
}


}}}
