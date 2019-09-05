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
#include <algorithm>


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
		float num = 0.0f;
		for (size_t i = 0; i < BinNumber; ++i)
		{
			const auto & mm = std::minmax(bins[i], hist.bins[i]);
			num += ( 1.0f - (1.0f + mm.first) / (1.0f + mm.second) );
		}
		return num / (BinNumber - 1);
	}

	BinValue_t getBinValue(const std::size_t binNumber) const noexcept
	{
		return (binNumber < BinNumber) ? bins[binNumber] : 0;
	}

	void setBinValue(const std::size_t binNumber, const BinValue_t value) noexcept
	{
		if (binNumber < BinNumber) bins[binNumber] = value;
	}

private:
	std::array<BinValue_t, BinNumber> bins{};
};


}}}

#endif /* LOGIC_IMAGES_HISTOGRAMT_HPP_ */
