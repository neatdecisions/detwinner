/*
 ===============================================================================
 Name        : IconManager.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <tools/IconManager.hpp>

namespace detwinner::tools {

//------------------------------------------------------------------------------
IconManager::IconManager() : m_iconTheme(Gtk::IconTheme::get_default()) {}

//------------------------------------------------------------------------------
IconManager::~IconManager() = default;

//------------------------------------------------------------------------------
IconManager &
IconManager::GetInstance()
{
	static IconManager iconManager;
	return iconManager;
}

//------------------------------------------------------------------------------
Glib::RefPtr<Gdk::Pixbuf>
IconManager::getFileIcon(const std::string & filePath, int size) const
{
	Glib::RefPtr<Gdk::Pixbuf> result;
	const Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(filePath);
	Glib::RefPtr<Gio::FileInfo> fileInfo = file->query_info("standard::icon");
	if (fileInfo)
	{
		const Glib::RefPtr<Gio::Icon> icon = fileInfo->get_icon();
		if (icon) result = IconManager::GetInstance().getBuiltinIcon(icon, size);
	}
	return result;
}

//------------------------------------------------------------------------------
Glib::RefPtr<Gdk::Pixbuf>
IconManager::getBuiltinIcon(const std::string & iconName, int size) const
{
	Glib::RefPtr<Gdk::Pixbuf> result;

	if (m_iconTheme)
	{
		const Gtk::IconInfo iconInfo = m_iconTheme->lookup_icon(iconName, size, Gtk::ICON_LOOKUP_USE_BUILTIN);
		if (iconInfo) result = iconInfo.load_icon();
	}
	return result;
}

//------------------------------------------------------------------------------
Glib::RefPtr<Gdk::Pixbuf>
IconManager::getBuiltinIcon(const Glib::RefPtr<Gio::Icon> & icon, int size) const
{
	Glib::RefPtr<Gdk::Pixbuf> result;

	if (m_iconTheme && icon)
	{
		const Gtk::IconInfo iconInfo = m_iconTheme->lookup_icon(icon, size, Gtk::ICON_LOOKUP_USE_BUILTIN);
		if (iconInfo) result = iconInfo.load_icon();
	}
	return result;
}

} // namespace detwinner::tools
