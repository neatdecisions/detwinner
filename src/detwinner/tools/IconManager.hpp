/*
 ===============================================================================
 Name        : IconManager.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>

namespace detwinner::tools {

class IconManager
{
public:
	static IconManager & GetInstance();
	Glib::RefPtr<Gdk::Pixbuf> getFileIcon(const std::string & filePath, int size) const;
	Glib::RefPtr<Gdk::Pixbuf> getBuiltinIcon(const std::string & iconName, int size) const;
	Glib::RefPtr<Gdk::Pixbuf> getBuiltinIcon(const Glib::RefPtr<Gio::Icon> & icon, int size) const;

private:
	Glib::RefPtr<Gtk::IconTheme> m_iconTheme;
	IconManager();
	~IconManager();
};

} // namespace detwinner::tools
