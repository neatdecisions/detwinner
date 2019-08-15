/*
 ===============================================================================
 Name        : Initializer.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/Initializer.hpp>

#include <Magick++.h>
#include <magick/magick.h>
#include <vector>


namespace detwinner {
namespace logic {


void Initialize()
{
	Magick::InitializeMagick(nullptr);

	const std::vector<std::string> aUnsupportedFormats = {
		"AVI",
		"EPDF",
		"EPI",
		"EPS",
		"EPT",
		"EPT2",
		"EPT3",
		"EPSF",
		"EPSI",
		"GRAY",
		"HTM",
		"HTML",
		"M2V",
		"META",
		"MPEG",
		"MPG",
		"PALM",
		"PDF",
		"PS",
		"PS2",
		"PS3",
		"SHTML",
		"TEXT",
		"TILE",
		"TIM",
		"TOPOL",
		"TRIO",
		"TTF",
		"TXT",
		"UIL",
		"URL",
		"UYVY",
		"VICAR",
		"VID",
		"VIFF",
		"WBMP",
		"WMF",
		"WPG",
		"XPM"
	};

	// first call GetMagickInfo, otherwise UnregisterMagickInfo won't work
	MagickLib::ExceptionInfo e;
	MagickLib::GetExceptionInfo(&e);
	MagickLib::GetMagickInfo("*", &e);

	for (const std::string & value : aUnsupportedFormats)
	{
		MagickLib::UnregisterMagickInfo(value.c_str());
	}


}


}}
