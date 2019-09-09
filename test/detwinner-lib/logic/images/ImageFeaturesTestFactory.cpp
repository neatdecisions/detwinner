#include "ImageFeaturesTestFactory.hpp"

#include <array>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
ImageFeatures ImageFeaturesTestFactory::CreateFeatures_1()
{
	ImageFeatures feat1(0, 125.0f / 80.0f);
	const std::array< std::array<Histogram::BinValue_t, Histogram::kBinCount>, 4 > yHistograms1 = { {
		{ { 139, 29, 0, 0, 29, 16, 8, 17, 4, 41, 15, 26, 15, 26, 51, 28, 14, 15, 12, 21, 13, 23, 14, 20, 13, 21, 7, 25, 22, 18, 6, 1792 } },
		{ { 165, 38, 0, 0, 29, 17, 26, 17, 4, 38, 22, 26, 25, 27, 69, 31, 15, 13, 24, 24, 17, 16, 16, 15, 18, 22, 14, 15, 17, 15, 8, 1697 } },
		{ { 55, 17, 0, 0, 28, 9, 35, 21, 29, 39, 31, 63, 8, 53, 401, 6, 17, 5, 10, 9, 7, 6, 11, 13, 18, 7, 14, 11, 12, 4, 6, 1535 } },
		{ { 60, 17, 0, 0, 26, 16, 12, 16, 22, 26, 17, 44, 5, 39, 239, 12, 15, 9, 4, 9, 9, 4, 7, 9, 11, 6, 13, 6, 10, 8, 8, 1801 } }
	} };

	const std::array< std::array<Histogram::BinValue_t, Histogram::kBinCount>, 4 > uHistograms1 = { {
		{ { 0, 0, 0, 0, 0, 0, 0, 3, 57, 67, 79, 44, 52, 111, 62, 79, 1926, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 0, 0, 6, 59, 101, 69, 44, 75, 131, 49, 90, 1856, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 0, 0, 1, 13, 29, 17, 31, 16, 520, 121, 116, 1616, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 0, 0, 10, 25, 19, 21, 22, 20, 314, 75, 94, 1880, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
	} };

	const std::array< std::array<Histogram::BinValue_t, Histogram::kBinCount>, 4 > vHistograms1 = { {
		{ { 0, 0, 0, 0, 0, 38, 5, 10, 3, 6, 8, 8, 5, 6, 187, 278, 1926, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 66, 5, 5, 7, 7, 2, 5, 5, 3, 219, 300, 1856, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 401, 42, 48, 29, 34, 7, 41, 22, 15, 63, 162, 1616, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 238, 21, 34, 15, 25, 9, 15, 19, 10, 77, 137, 1880, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
	} };

	const std::array< std::array<HistogramI::BinValue_t, HistogramI::kBinCount>, 4 > iHistograms1 = { {
		{ { 1688, 0, 0, 0, 0, 0, 0, 792 } },
		{ { 1643, 0, 0, 0, 0, 0, 0, 837 } },
		{ { 1473, 0, 0, 1, 1, 0, 0, 1005 } },
		{ { 1771, 0, 0, 0, 0, 0, 0, 709 } }
	} };

	for (std::size_t j = 0; j < 4; ++j)
	{
		for (std::size_t i = 0; i < Histogram::kBinCount; ++i)
		{
			feat1.histY[j].setBinValue(i, yHistograms1[j][i]);
			feat1.histU[j].setBinValue(i, uHistograms1[j][i]);
			feat1.histV[j].setBinValue(i, vHistograms1[j][i]);
		}
		for (std::size_t i = 0; i < HistogramI::kBinCount; ++i)
		{
			feat1.histI[j].setBinValue(i, iHistograms1[j][i]);
		}
	}

	return feat1;
}


//------------------------------------------------------------------------------
ImageFeatures ImageFeaturesTestFactory::CreateFeatures_2()
{
	ImageFeatures feat2(1, 126.0f / 80.0f);
	const std::array< std::array<Histogram::BinValue_t, Histogram::kBinCount>, 4 > yHistograms2 = { {
		{ { 54, 3, 4, 9, 10, 12, 18, 27, 24, 35, 34, 45, 45, 76, 68, 334, 21, 5, 6, 14, 9, 6, 13, 15, 16, 12, 3, 18, 11, 4, 5, 1564 } },
		{ { 56, 3, 7, 8, 11, 14, 12, 17, 18, 24, 30, 26, 38, 44, 42, 223, 15, 8, 9, 5, 8, 14, 7, 8, 8, 11, 7, 15, 5, 8, 10, 1809 } },
		{ { 106, 28, 22, 9, 23, 14, 14, 19, 16, 19, 24, 19, 22, 30, 28, 43, 21, 24, 28, 21, 19, 25, 22, 24, 16, 21, 22, 24, 20, 23, 15, 1759 } },
		{ { 115, 31, 21, 23, 33, 15, 15, 25, 23, 21, 20, 24, 38, 23, 28, 76, 22, 18, 20, 27, 25, 17, 21, 20, 14, 19, 32, 15, 17, 16, 18, 1688 } }
	} };

	const std::array< std::array<Histogram::BinValue_t, Histogram::kBinCount>, 4 > uHistograms2 = { {
		{ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 29, 37, 34, 436, 207, 148, 1627, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 38, 32, 34, 281, 141, 126, 1862, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 55, 117, 109, 138, 124, 148, 1821, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 0, 0, 0, 1, 0, 9, 71, 120, 116, 164, 132, 132, 1775, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
	} };

	const std::array< std::array<Histogram::BinValue_t, Histogram::kBinCount>, 4 > vHistograms2 = { {
		{ { 0, 0, 0, 0, 36, 348, 72, 45, 33, 20, 38, 25, 67, 71, 69, 67, 1629, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 21, 219, 44, 32, 18, 14, 22, 34, 62, 61, 58, 72, 1861, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 5, 24, 12, 14, 13, 5, 6, 32, 102, 171, 161, 145, 1828, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
		{ { 0, 0, 0, 0, 7, 51, 6, 12, 8, 4, 7, 21, 143, 164, 163, 160, 1773, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
	} };

	const std::array< std::array<HistogramI::BinValue_t, HistogramI::kBinCount>, 4 > iHistograms2 = { {
		{ { 1794, 0, 0, 0, 0, 0, 0, 726 } },
		{ { 1995, 0, 0, 0, 0, 0, 0, 525 } },
		{ { 1992, 0, 0, 0, 0, 0, 0, 528 } },
		{ { 1963, 0, 0, 0, 0, 0, 0, 557 } },
	} };

	for (std::size_t j = 0; j < 4; ++j)
	{
		for (std::size_t i = 0; i < Histogram::kBinCount; ++i)
		{
			feat2.histY[j].setBinValue(i, yHistograms2[j][i]);
			feat2.histU[j].setBinValue(i, uHistograms2[j][i]);
			feat2.histV[j].setBinValue(i, vHistograms2[j][i]);
		}
		for (std::size_t i = 0; i < HistogramI::kBinCount; ++i)
		{
			feat2.histI[j].setBinValue(i, iHistograms2[j][i]);
		}
	}

	return feat2;
}


}}}
