/*
 ===============================================================================
 Name        : DetwinnerWindow.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef UI_DETWINNERWINDOW_HPP_
#define UI_DETWINNERWINDOW_HPP_

#include <gtkmm.h>

#include <gtkmm/applicationwindow.h>

#include <settings/SearchSettings.hpp>
#include <ui/SearchOptionsPane.hpp>
#include <ui/SearchResultsPane.hpp>


namespace detwinner {
namespace ui {


class DetwinnerWindow: public Gtk::ApplicationWindow
{

public:
	DetwinnerWindow();

protected:
	void on_start_search();
	void on_search_mode_changed(settings::SearchSettings::SearchMode_t mode);

	Gtk::Stack m_Stack;
	SearchOptionsPane m_SearchOptionsBox;
	SearchResultsPane m_SearchResultsBox;
	Glib::RefPtr<Gtk::Builder> m_refBuilder;
	Gtk::HeaderBar * m_pHeaderBarOptions = nullptr;
	Gtk::HeaderBar * m_pHeaderBarResults = nullptr;
	Gtk::Label * m_pLabelSearchMode = nullptr;

	static const Glib::ustring kName_SearchOptions;
	static const Glib::ustring kName_SearchResults;
};


}}

#endif // UI_DETWINNERWINDOW_HPP_
