/*
 ===============================================================================
 Name        : PermanentFileDeleter.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <tools/PermanentFileDeleter.hpp>

#include <giomm.h>
#include <glibmm.h>

namespace detwinner::tools {

//------------------------------------------------------------------------------
bool
PermanentFileDeleter::removeFile(const std::string & filePath)
{
	bool isFileDeleted = false;
	const Glib::RefPtr<Gio::File> & file = Gio::File::create_for_path(filePath);
	if (file)
	{
		try
		{
			isFileDeleted = !file->query_exists() || file->remove();
		} catch (const Glib::Error & e)
		{
			g_warning("Unable to delete file %s: %s", filePath.c_str(), e.what().c_str());
		} catch (...)
		{
			g_warning("Unable to delete file %s: unknown error", filePath.c_str());
		}
	}
	return isFileDeleted;
}

} // namespace detwinner::tools
