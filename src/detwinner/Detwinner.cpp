/*
 ===============================================================================
 Name        : Detwinner.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <DetwinnerApp.hpp>

#include <locale.h>
#include <glib/gi18n.h>

#include <config.h>


int main(int argc, char *argv [])
{
	bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
	setlocale(LC_ALL, "");
	return detwinner::DetwinnerApp::Create()->run(argc, argv);
}
