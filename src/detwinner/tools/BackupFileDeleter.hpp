/*
 ===============================================================================
 Name        : BackupFileDeleter.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <tools/AbstractFileDeleter.hpp>

namespace Gtk {
class Window;
}

namespace detwinner::tools {

class BackupFileDeleter : public AbstractFileDeleter
{
public:
	BackupFileDeleter(const std::string & backupFolder, Gtk::Window * dialogParent);
	virtual bool removeFile(const std::string & filePath) override;

private:
	bool move(const std::string & from, const std::string & to);
	bool prepareDirectory(const std::string & backupFileName);
	const std::string m_backupFolder;
	Gtk::Window * m_dialogParent;
	bool m_cancelMode;
};

} // namespace detwinner::tools
