/*
 ===============================================================================
 Name        : SearchResultsPane.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>

#include <callbacks/IDuplicateReceiver.hpp>
#include <settings/SearchSettings.hpp>
#include <ui/DuplicatesTreeView.hpp>
#include <ui/FilePreview.hpp>

namespace detwinner::ui {

class SearchResultsPane : public Gtk::Box
{
public:
	SearchResultsPane();
	void init();
	void updateStatus();
	void setMode(settings::SearchSettings::SearchMode mode);
	Glib::RefPtr<Gio::SimpleActionGroup> getActionGroup();

	callbacks::IDuplicateReceiver::Ptr createPopulationDelegate();

private:
	void on_duplicate_selected(const Glib::ustring & filePath, const Glib::ustring & filePathLocked);

	void on_smart_select_first_in_group_clicked();
	void on_smart_select_last_in_group_clicked();
	void on_smart_select_earliest_modified_clicked();
	void on_smart_select_latest_modified_clicked();
	void on_smart_select_name_shortest_clicked();
	void on_smart_select_name_longest_clicked();
	void on_smart_select_path_shortest_clicked();
	void on_smart_select_path_longest_clicked();
	void on_smart_select_resolution_lowest_clicked();
	void on_smart_select_resolution_highest_clicked();

	callbacks::IDeferredAction::Result execute_duplicate_action(const Glib::ustring & label,
	                                                            callbacks::IDeferredAction::Ptr action);

	void on_expand_all_clicked();
	void on_collapse_all_clicked();
	void on_sort_by_changed(int value);
	void on_sort_ascending_toggled();
	void sort_tree();

	void on_select_all();
	void on_select_none();
	void on_select_invert();

	void on_delete_permanently();
	void on_delete_move_to_trash();
	void on_delete_move_to_folder();
	void process_deletion_result(callbacks::IDeferredAction::Result result);

	Gtk::ScrolledWindow m_scrolledWindow;

	FilePreview m_filePreviewLeft;
	FilePreview m_filePreviewRight;

	Gtk::Frame m_mainFrame;
	Gtk::Paned m_mainPanel;
	Gtk::Paned m_previewPanel;

	Gtk::Statusbar m_statusBar;

	Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;

	Glib::RefPtr<Gio::SimpleAction> m_refActionSortBy;
	Glib::RefPtr<Gio::SimpleAction> m_refActionSortAscending;

	DuplicatesTreeView m_treeView;
};

} // namespace detwinner::ui
