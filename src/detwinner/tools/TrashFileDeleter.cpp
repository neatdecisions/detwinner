/*
 ===============================================================================
 Name        : TrashFileDeleter.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <tools/TrashFileDeleter.hpp>

#include <gtkmm.h>


namespace detwinner {
namespace tools {


//------------------------------------------------------------------------------
bool
TrashFileDeleter::removeFile(const std::string & filePath)
{
	bool isFileDeleted = false;
	const Glib::RefPtr<Gio::File> & file = Gio::File::create_for_path(filePath);
	if (file)
	{
		try
		{
			isFileDeleted = !file->query_exists() || file->trash();
		} catch (const Glib::Error & e)
		{
			g_warning("Unable to delete file %s to trash - error %d: %s", filePath.c_str(), e.code(), e.what().c_str());
		} catch (...)
		{
			g_warning("Unable to delete file %s to trash: unknown error", filePath.c_str());
		}
	}
	return isFileDeleted;
}


}}
