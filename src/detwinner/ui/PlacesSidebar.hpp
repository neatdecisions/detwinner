/*
 ===============================================================================
 Name        : PlacesSidebar.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <giomm/file.h>
#include <giomm/volume.h>
#include <glibmm/ustring.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

namespace detwinner::ui {

class PlacesSidebar : public Gtk::ScrolledWindow
{
public:
	PlacesSidebar(PlacesSidebar &&) = default;
	PlacesSidebar & operator=(PlacesSidebar &&) = default;

	PlacesSidebar(const PlacesSidebar &) = delete;
	PlacesSidebar & operator=(const PlacesSidebar &) = delete;

	PlacesSidebar();

	Glib::RefPtr<const Gio::File> getLocation() const { return m_currentLocation; }

	using SignalOpenLocation_t = sigc::signal<void, const Glib::RefPtr<const Gio::File> &>;
	SignalOpenLocation_t signalOpenLocation() { return m_signalOpenLocation; }

private:
	void setLocation(const Glib::RefPtr<const Gio::File> & location);
	void update();
	void onRowActivated(Gtk::ListBoxRow * row);
	void onMountFinished(Glib::RefPtr<Gio::AsyncResult> & asyncResult);
	void onDrawListBoxHeader(Gtk::ListBoxRow * row, Gtk::ListBoxRow * before);

	SignalOpenLocation_t m_signalOpenLocation;
	Glib::RefPtr<const Gio::File> m_currentLocation;
	Gtk::ListBox m_listBox;
	Glib::RefPtr<Gio::VolumeMonitor> m_volumeMonitor;
	Glib::RefPtr<Gio::Volume> m_volumeBeingMounted;
};

} // namespace detwinner::ui
