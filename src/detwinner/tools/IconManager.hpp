/*
 ===============================================================================
 Name        : IconManager.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef TOOLS_ICONMANAGER_HPP_
#define TOOLS_ICONMANAGER_HPP_

#include <gtkmm.h>


namespace detwinner {
namespace tools {


class IconManager
{
public:
	static IconManager & GetInstance();
	Glib::RefPtr<Gdk::Pixbuf> getFileIcon(const std::string & filePath, int size);
	Glib::RefPtr<Gdk::Pixbuf> getBuiltinIcon(const std::string & iconName, int size);
	Glib::RefPtr<Gdk::Pixbuf> getBuiltinIcon(const Glib::RefPtr<Gio::Icon> & icon, int size);

private:
	Glib::RefPtr<Gtk::IconTheme> m_iconTheme;
	IconManager();
	~IconManager();
};

}}

#endif /* TOOLS_ICONMANAGER_HPP_ */
