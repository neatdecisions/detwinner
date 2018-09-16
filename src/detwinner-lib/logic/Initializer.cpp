/*
 ===============================================================================
 Name        : Initializer.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
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
	Magick::InitializeMagick(0);

	const std::vector<std::string> aUnsupportedFormats = {
		"AVI",
		"GRAY",
		"HTML"
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

	for (const std::string & value : aUnsupportedFormats)
	{
		MagickLib::UnregisterMagickInfo(value.c_str());
	}

}


}}
