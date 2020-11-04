/*
 ===============================================================================
 Name        : SearchOptionsPane.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef UI_SEARCHOPTIONSPANE_HPP_
#define UI_SEARCHOPTIONSPANE_HPP_

#include <gtkmm.h>
#include <settings/SearchSettingsManager.hpp>
#include <ui/FileTreeView.hpp>


namespace detwinner {
namespace ui {


class SearchOptionsPane : public Gtk::Box
{
public:
	SearchOptionsPane();
	Glib::RefPtr<Gio::SimpleActionGroup> getActionGroup();

	settings::SearchSettings getSearchSettings() const;
	std::vector<std::string> getSearchPaths() const;

	using signal_search_mode_changed_t = sigc::signal<void, settings::SearchSettings::SearchMode_t>;
	signal_search_mode_changed_t signal_search_mode_changed();

private:
	void on_open_location(const Glib::RefPtr<Gio::File> & location, Gtk::PlacesOpenFlags open_flags);
	void on_select_all_clicked();
	void on_clear_selection_clicked();
	void on_refresh_clicked();
	void on_show_hidden_toggled();
	void on_search_mode_changed(int iValue);
	void on_search_settings_default_clicked();
	void on_search_settings_similar_images_clicked();
	void on_search_settings_exact_duplicates_clicked();
	void show_search_settings_dialog(settings::SearchSettings::SearchMode_t mode);
	settings::SearchSettings::SearchMode_t getSearchModeByInt(int value) const;
	int getIntBySearchMode(settings::SearchSettings::SearchMode_t value) const;

	Gtk::PlacesSidebar m_places;
	Gtk::ScrolledWindow m_scrolledWindow;

	Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;
	Glib::RefPtr<Gio::SimpleAction> m_refActionShowHidden;
	Glib::RefPtr<Gio::SimpleAction> m_refActionSearchMode;
	signal_search_mode_changed_t m_signalSearchModeChanged;
	settings::SearchSettingsManager m_searchSettingsManager;
	FileTreeView m_fileTree;
};


}}

#endif /* UI_SEARCHOPTIONSPANE_HPP_ */
