/*
 ===============================================================================
 Name        : SearchOptionsPane.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>

#include <settings/SearchSettingsManager.hpp>
#include <ui/FileTreeView.hpp>
#include <ui/PlacesSidebar.hpp>

namespace detwinner::ui {

class SearchOptionsPane : public Gtk::Box
{
public:
	SearchOptionsPane();
	Glib::RefPtr<Gio::SimpleActionGroup> getActionGroup();

	settings::SearchSettings getSearchSettings() const;
	std::vector<std::string> getSearchPaths() const;

	using signal_search_mode_changed_t = sigc::signal<void, settings::SearchSettings::SearchMode>;
	signal_search_mode_changed_t signal_search_mode_changed();

private:
	void on_open_location(const Glib::RefPtr<const Gio::File> & location);
	void on_select_all_clicked();
	void on_clear_selection_clicked();
	void on_refresh_clicked();
	void on_show_hidden_toggled();
	void on_search_mode_changed(int searchMode);
	void on_search_settings_default_clicked();
	void on_search_settings_similar_images_clicked();
	void on_search_settings_exact_duplicates_clicked();
	void show_search_settings_dialog(settings::SearchSettings::SearchMode mode);

	PlacesSidebar m_places;
	Gtk::ScrolledWindow m_scrolledWindow;

	Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;
	Glib::RefPtr<Gio::SimpleAction> m_refActionShowHidden;
	Glib::RefPtr<Gio::SimpleAction> m_refActionSearchMode;
	signal_search_mode_changed_t m_signalSearchModeChanged;
	settings::SearchSettingsManager m_searchSettingsManager;
	FileTreeView m_fileTree;
};

} // namespace detwinner::ui
