/*
 ===============================================================================
 Name        : BackupFileDeleter.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <tools/BackupFileDeleter.hpp>

#include <glibmm.h>
#include <glibmm/i18n.h>
#include <gtkmm.h>

namespace detwinner::tools {

//------------------------------------------------------------------------------
BackupFileDeleter::BackupFileDeleter(const std::string & backupFolder, Gtk::Window * dialogParent)
		: m_backupFolder(backupFolder), m_dialogParent(dialogParent), m_cancelMode(false)
{
}

//------------------------------------------------------------------------------
bool
BackupFileDeleter::removeFile(const std::string & filePath)
{
	if (m_cancelMode) return false;
	const std::string & backupFilePath = Glib::build_filename(m_backupFolder, filePath);

	if (!prepareDirectory(backupFilePath)) return false;

	if (Glib::file_test(backupFilePath, Glib::FILE_TEST_EXISTS))
	{
		Gtk::MessageDialog messageDialog(_("File with this name already exists. What should we do?"), false,
		                                 Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE, true);
		messageDialog.set_secondary_text(backupFilePath);
		messageDialog.set_secondary_text(filePath, false);
		messageDialog.add_button(_("Overwrite"), Gtk::RESPONSE_YES);
		messageDialog.add_button(_("Skip"), Gtk::RESPONSE_NO);
		messageDialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
		if (m_dialogParent) messageDialog.set_transient_for(*m_dialogParent);

		switch (messageDialog.run())
		{
		case Gtk::RESPONSE_YES:
			break;
		case Gtk::RESPONSE_CANCEL:
			m_cancelMode = true;
			return false;
		default:
			return false;
		}
	}

	return move(filePath, backupFilePath);
}

//------------------------------------------------------------------------------
bool
BackupFileDeleter::prepareDirectory(const std::string & backupFileName)
{
	const std::string & backupFileDirName = Glib::path_get_dirname(backupFileName);
	bool result = Glib::file_test(backupFileDirName, Glib::FILE_TEST_IS_DIR);
	if (!result)
	{
		const Glib::RefPtr<Gio::File> & dir = Gio::File::create_for_path(backupFileDirName);
		if (dir)
		{
			try
			{
				result = dir->make_directory_with_parents();
			} catch (const Glib::Error & e)
			{
				g_warning("Can't use backup directory %s: %s", backupFileDirName.c_str(), e.what().c_str());
			} catch (...)
			{
				g_warning("Can't use backup directory %s", backupFileDirName.c_str());
			}
		}
	}
	return result;
}

//------------------------------------------------------------------------------
bool
BackupFileDeleter::move(const std::string & from, const std::string & to)
{
	bool isFileMoved = false;
	const Glib::RefPtr<Gio::File> & fileFrom = Gio::File::create_for_path(from);
	const Glib::RefPtr<Gio::File> & fileTo = Gio::File::create_for_path(to);
	if (fileFrom && fileTo)
	{
		try
		{
			isFileMoved = !fileFrom->query_exists() || fileFrom->move(fileTo, Gio::FILE_COPY_OVERWRITE);
		} catch (const Glib::Error & e)
		{
			g_warning("Unable to move file %s to %s: %s", from.c_str(), to.c_str(), e.what().c_str());
		} catch (...)
		{
			g_warning("Unable to move file %s to %s: unknown error", from.c_str(), to.c_str());
		}
	}
	return isFileMoved;
}

} // namespace detwinner::tools
