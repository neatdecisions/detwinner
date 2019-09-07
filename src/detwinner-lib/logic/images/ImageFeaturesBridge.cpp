/*
 ===============================================================================
 Name        : ImageFeaturesBridge.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/ImageFeaturesBridge.hpp>

#include <cmath>


namespace detwinner {
namespace logic {
namespace images {


//------------------------------------------------------------------------------
template <std::size_t BinCount>
void
ImageFeaturesBridge::NormalizeHistogramFromArrayT(
	const std::array<std::size_t, BinCount> & realHist,
	HistogramT<BinCount> & hist) noexcept
{
	for (std::size_t i = 0; i < BinCount; ++i)
	{
		hist.setBinValue(i, realHist[i]);
	}
}


//------------------------------------------------------------------------------
void
ImageFeaturesBridge::GetIntensityHistogram(
	const Magick::Image & image,
	const Magick::Geometry & roi,
	HistogramI & histI)
{
	const unsigned int width = roi.xOff() + roi.width();
	const unsigned int height = roi.yOff() + roi.height();

	if (width > image.size().width() || height > image.size().height())
	{
		throw "Logic exception";
	}

	constexpr int kBinNumber = HistogramI::kBinCount;
	constexpr float kBinSize = 255.0f / kBinNumber;

	std::array<std::size_t, kBinNumber> realHistI{};

	for (unsigned int x = roi.xOff(); x < width; ++x)
	{
		for (unsigned int y = roi.yOff(); y < height; ++y)
		{
			const Magick::Color & color = image.pixelColor(x, y);
			int n = static_cast<int>(std::floor(( color.intensity() ) / kBinSize));
			if (n >= kBinNumber) n = kBinNumber - 1;
			if (n < 0) n = 0;
			++realHistI[n];
		}
	}
	NormalizeHistogramFromArrayT(realHistI, histI);
}


//------------------------------------------------------------------------------
void
ImageFeaturesBridge::GetYUVHistograms(
	const Magick::Image & image,
	const Magick::Geometry & roi,
	Histogram & histY,
	Histogram & histU,
	Histogram & histV)
{
	constexpr float yMax = 1.0f;
	constexpr float uMax = 0.436f;
	constexpr float vMax = 0.615f;
	constexpr float yMin = 0.0f;
	constexpr float uMin = -uMax;
	constexpr float vMin = -vMax;

	constexpr int kBinNumber = Histogram::kBinCount;

	constexpr float yBinSize = (yMax - yMin) / kBinNumber;
	constexpr float uBinSize = (uMax - uMin) / kBinNumber;
	constexpr float vBinSize = (vMax - vMin) / kBinNumber;

	const unsigned int width = roi.xOff() + roi.width();
	const unsigned int height = roi.yOff() + roi.height();


	if (width > image.size().width() || height > image.size().height())
	{
		throw "Unknown error";
	}

	std::array<std::size_t, kBinNumber> realHistY {};
	std::array<std::size_t, kBinNumber> realHistU {};
	std::array<std::size_t, kBinNumber> realHistV {};

	for (unsigned int x = roi.xOff(); x < width; ++x)
	{
		for (unsigned int y = roi.yOff(); y < height; ++y)
		{
			const Magick::ColorYUV & color = image.pixelColor(x, y);
			const float yyy = ( color.alpha() > 0.8 ) ? 1.0 : color.y();
			int n = static_cast<int>(floor(( yyy - yMin ) / yBinSize));
			if (n >= kBinNumber) n = kBinNumber - 1;
			if (n < 0) n = 0;
			++realHistY[n];

			float t = static_cast<float>(color.u());
			if (t < uMin) t = uMin;
			if (t > uMax) t = uMax;

			n = static_cast<int>(std::floor(( t - uMin ) / uBinSize));
			if (n >= kBinNumber) n = kBinNumber - 1;
			if (n < 0) n = 0;
			++realHistU[n];

			t = static_cast<float>(color.v());
			if (t < vMin) t = vMin;
			if (t > vMax) t = vMax;

			n = static_cast<int>(floor(( t - vMin ) / vBinSize));
			if (n >= kBinNumber) n = kBinNumber - 1;
			if (n < 0) n = 0;
			++realHistV[n];
		}
	}

	NormalizeHistogramFromArrayT(realHistY, histY);
	NormalizeHistogramFromArrayT(realHistU, histU);
	NormalizeHistogramFromArrayT(realHistV, histV);
}


//------------------------------------------------------------------------------
ImageFeatures
ImageFeaturesBridge::GetImageFeatures(Magick::Image & image, unsigned int id)
{
	constexpr unsigned int kImageDimension = 128;
	static const Magick::Geometry kResizeGeometry(kImageDimension, kImageDimension);

	Magick::Geometry imageSize = image.size();
	ImageFeatures feats(id, static_cast<float>(imageSize.height()) / static_cast<float>(imageSize.width()));

	if (imageSize.height() > kImageDimension || imageSize.width() > kImageDimension)
	{
		image.thumbnail(kResizeGeometry);
		imageSize = image.size();
	}

	const unsigned int dw = imageSize.width() / 2;
	const unsigned int dw1 = dw + 2 * dw % 2;
	const unsigned int dh = imageSize.height() / 2;
	const unsigned int dh1 = dh + 2 * dh % 2;

	GetYUVHistograms(image, Magick::Geometry(dw, dh, 0, 0), feats.histY[0], feats.histU[0], feats.histV[0]);
	GetYUVHistograms(image, Magick::Geometry(dw, dh, dw1, 0), feats.histY[1], feats.histU[1], feats.histV[1]);
	GetYUVHistograms(image, Magick::Geometry(dw, dh, dw1, dh1), feats.histY[2], feats.histU[2], feats.histV[2]);
	GetYUVHistograms(image, Magick::Geometry(dw, dh, 0, dh1), feats.histY[3], feats.histU[3], feats.histV[3]);

	image.edge();

	GetIntensityHistogram(image, Magick::Geometry(dw, dh, 0, 0), feats.histI[0]);
	GetIntensityHistogram(image, Magick::Geometry(dw, dh, dw1, 0), feats.histI[1]);
	GetIntensityHistogram(image, Magick::Geometry(dw, dh, dw1, dh1), feats.histI[2]);
	GetIntensityHistogram(image, Magick::Geometry(dw, dh, 0, dh1), feats.histI[3]);

	return feats;
}


}}}
