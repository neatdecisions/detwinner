/*
 ===============================================================================
 Name        : ImageFeaturesBridge.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/images/ImageFeaturesBridge.hpp>

#include <algorithm>
#include <cmath>
#include <span>

namespace detwinner::logic::images {

//------------------------------------------------------------------------------
void
ImageFeaturesBridge::GetIntensityHistogram(const Magick::Image & image,
                                           const Magick::Geometry & roi,
                                           HistogramI & histI)
{
	const unsigned int width = roi.xOff() + roi.width();
	const unsigned int height = roi.yOff() + roi.height();

	if (width > image.size().width() || height > image.size().height())
	{
		throw std::logic_error("Logic exception");
	}

	constexpr int kBinNumber = HistogramI::kBinCount;
	constexpr float kBinSize = MaxRGBFloat / kBinNumber;

	std::fill(histI.bins.begin(), histI.bins.end(), 0U);

	std::span<const Magick::PixelPacket> pixels(image.getConstPixels(roi.xOff(), roi.yOff(), roi.width(), roi.height()),
	                                            roi.width() * roi.height());

	for (const Magick::PixelPacket & pixel : pixels)
	{
		const int n =
				std::clamp(static_cast<int>(std::floor(Magick::Color(pixel).intensity() / kBinSize)), 0, kBinNumber - 1);
		++histI.bins[n];
	}
}

//------------------------------------------------------------------------------
void
ImageFeaturesBridge::GetYUVHistograms(
		const Magick::Image & image, const Magick::Geometry & roi, Histogram & histY, Histogram & histU, Histogram & histV)
{
	constexpr float kMaxY = 1.0f;
	constexpr float kMaxU = 0.436f;
	constexpr float kMaxV = 0.615f;
	constexpr float kMinY = 0.0f;
	constexpr float kMinU = -kMaxU;
	constexpr float kMinV = -kMaxV;

	constexpr int kBinNumber = Histogram::kBinCount;

	constexpr float kBinSizeY = (kMaxY - kMinY) / kBinNumber;
	constexpr float kBinSizeU = (kMaxU - kMinU) / kBinNumber;
	constexpr float kBinSizeV = (kMaxV - kMinV) / kBinNumber;

	const unsigned int width = roi.xOff() + roi.width();
	const unsigned int height = roi.yOff() + roi.height();

	if (width > image.size().width() || height > image.size().height())
	{
		throw std::logic_error("Unknown error");
	}

	std::fill(histY.bins.begin(), histY.bins.end(), 0U);
	std::fill(histU.bins.begin(), histU.bins.end(), 0U);
	std::fill(histV.bins.begin(), histV.bins.end(), 0U);

	std::span<const Magick::PixelPacket> pixels(image.getConstPixels(roi.xOff(), roi.yOff(), roi.width(), roi.height()),
	                                            roi.width() * roi.height());

	for (const Magick::PixelPacket & pixel : pixels)
	{
		const Magick::ColorYUV color = Magick::Color(pixel);
		if (color.alpha() > 0.8) continue;

		const float valY = std::clamp(static_cast<float>(color.y()), kMinY, kMaxY);
		int n = std::clamp(static_cast<int>(std::floor((valY - kMinY) / kBinSizeY)), 0, kBinNumber - 1);
		++histY.bins[n];

		const float valU = std::clamp(static_cast<float>(color.u()), kMinU, kMaxU);
		n = std::clamp(static_cast<int>(std::floor((valU - kMinU) / kBinSizeU)), 0, kBinNumber - 1);
		++histU.bins[n];

		const float valV = std::clamp(static_cast<float>(color.v()), kMinV, kMaxV);
		n = std::clamp(static_cast<int>(std::floor((valV - kMinV) / kBinSizeV)), 0, kBinNumber - 1);
		++histV.bins[n];
	}
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

	const Magick::Geometry section1(dw, dh, 0, 0);
	const Magick::Geometry section2(dw, dh, dw1, 0);
	const Magick::Geometry section3(dw, dh, dw1, dh1);
	const Magick::Geometry section4(dw, dh, 0, dh1);
	image.normalize();

	GetYUVHistograms(image, section1, feats.histY[0], feats.histU[0], feats.histV[0]);
	GetYUVHistograms(image, section2, feats.histY[1], feats.histU[1], feats.histV[1]);
	GetYUVHistograms(image, section3, feats.histY[2], feats.histU[2], feats.histV[2]);
	GetYUVHistograms(image, section4, feats.histY[3], feats.histU[3], feats.histV[3]);

	image.type(Magick::GrayscaleType);
	image.edge();

	GetIntensityHistogram(image, section1, feats.histI[0]);
	GetIntensityHistogram(image, section2, feats.histI[1]);
	GetIntensityHistogram(image, section3, feats.histI[2]);
	GetIntensityHistogram(image, section4, feats.histI[3]);

	return feats;
}

} // namespace detwinner::logic::images
