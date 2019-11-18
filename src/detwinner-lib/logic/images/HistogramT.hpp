/*
 ===============================================================================
 Name        : HistogramT.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_IMAGES_HISTOGRAMT_HPP_
#define LOGIC_IMAGES_HISTOGRAMT_HPP_


#include <array>
#include <numeric>


namespace detwinner {
namespace logic {
namespace images {


//==============================================================================
// HistogramT
//==============================================================================
template<std::size_t BinNumber>
class HistogramT
{
public:
	static_assert( BinNumber > 1, "Histogram should contain more than 1 bin" );
	using BinValue_t = uint_least16_t;
	static constexpr std::size_t kBinCount = BinNumber;

	float compare(const HistogramT & hist) const noexcept
	{
		// Hassanat distance
		constexpr auto aLbmAccumulator = [](float a, float b) { return a + b; };
		constexpr auto aLbmTransformer = [](BinValue_t a, BinValue_t b) {
			return ( a < b ? (1.0f + a) / (1.0f + b) :
			                 (1.0f + b) / (1.0f + a) );
		};

		return (BinNumber - std::inner_product(bins.begin(), bins.end(), hist.bins.begin(), 0.0f,
				aLbmAccumulator, aLbmTransformer)) / (BinNumber - 1);
	}

	std::array<BinValue_t, BinNumber> bins{};
};


}}}

#endif /* LOGIC_IMAGES_HISTOGRAMT_HPP_ */
