/*
 ===============================================================================
 Name        : HistogramT.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <array>
#include <cstdint>
#include <numeric>

namespace detwinner::logic::images {

//==============================================================================
// HistogramT
//==============================================================================
template <std::size_t BinNumber>
class HistogramT
{
public:
	static_assert(BinNumber > 1, "Histogram should contain more than 1 bin");
	using BinValue_t = uint_least16_t;
	static constexpr std::size_t kBinCount = BinNumber;

	float compare(const HistogramT & hist) const noexcept
	{
		// Hassanat distance
		constexpr auto aLbmAccumulator = [](float a, float b) { return a + b; };
		constexpr auto aLbmTransformer = [](BinValue_t a, BinValue_t b) {
			return (a < b ? (1.0f + a) / (1.0f + b) : (1.0f + b) / (1.0f + a));
		};

		return (BinNumber -
		        std::inner_product(bins.begin(), bins.end(), hist.bins.begin(), 0.0f, aLbmAccumulator, aLbmTransformer)) /
		       (BinNumber - 1);
	}

	std::array<BinValue_t, BinNumber> bins{};
};

} // namespace detwinner::logic::images
