/*
 ===============================================================================
 Name        : DetwinnerWindow.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/DetwinnerWindow.hpp>

#include <glibmm/i18n.h>
#include <glibmm/timer.h>
#include <logic/DuplicateFilesFinder.hpp>
#include <ui/SearchProgressDialog.hpp>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
DetwinnerWindow::DetwinnerWindow() :
	m_refBuilder(Gtk::Builder::create()),
	m_pHeaderBarOptions(nullptr),
	m_pHeaderBarResults(nullptr),
	m_pLabelSearchMode(nullptr)
{
	m_refBuilder->add_from_resource("/com/neatdecisions/detwinner/ui/toolbarResultsPane.glade");
	m_refBuilder->add_from_resource("/com/neatdecisions/detwinner/ui/toolbarSearchPane.glade");

	m_refBuilder->get_widget("headerBarSearchResults", m_pHeaderBarResults);
	m_pHeaderBarResults->insert_action_group("view", m_SearchResultsBox.getActionGroup());
	m_pHeaderBarResults->set_show_close_button(true);
	m_pHeaderBarResults->set_title("Detwinner");


	m_refBuilder->get_widget("headerBarSearchOptions", m_pHeaderBarOptions);
	m_pHeaderBarOptions->insert_action_group("view", m_SearchOptionsBox.getActionGroup());
	m_pHeaderBarOptions->set_show_close_button(true);
	m_pHeaderBarOptions->set_title("Detwinner");

	m_refBuilder->get_widget("hbSearchMode_Label", m_pLabelSearchMode);

	set_titlebar(*m_pHeaderBarOptions);

	set_border_width(0);
	add(m_Stack);

	m_Stack.add(m_SearchOptionsBox, "searchOptions");
	m_Stack.add(m_SearchResultsBox, "searchResults");

	set_default_size(640, 480);
	show_all_children();

	add_action("startSearch", sigc::mem_fun(*this, &DetwinnerWindow::on_start_search));

	on_search_mode_changed(m_SearchOptionsBox.getSearchSettings().searchMode);
	m_SearchOptionsBox.signal_search_mode_changed().connect( sigc::mem_fun(*this, &DetwinnerWindow::on_search_mode_changed) );
}


//------------------------------------------------------------------------------
void
DetwinnerWindow::on_start_search()
{
	if (m_Stack.get_visible_child_name() == "searchOptions")
	{
		m_SearchResultsBox.init();
		const settings::SearchSettings searchSettings = m_SearchOptionsBox.getSearchSettings();
		SearchProgressDialog dialog(
			*this,
			searchSettings,
			m_SearchOptionsBox.getSearchPaths(),
			m_SearchResultsBox.createPopulationDelegate());

		int res = dialog.run();

		if (res == Gtk::RESPONSE_OK)
		{
			m_SearchResultsBox.setMode(searchSettings.searchMode);
			m_SearchResultsBox.updateStatus();
			m_Stack.set_visible_child("searchResults");
			set_titlebar(*m_pHeaderBarResults);
		}
	} else
	{
		m_Stack.set_visible_child("searchOptions");
		set_titlebar(*m_pHeaderBarOptions);
		m_SearchResultsBox.init();
	}
}


//------------------------------------------------------------------------------
void
DetwinnerWindow::on_search_mode_changed(settings::SearchSettings::SearchMode_t mode)
{
	if (m_pLabelSearchMode != nullptr)
	{
		if (mode == settings::SearchSettings::SearchMode_t::kSimilarImages)
		{
			m_pLabelSearchMode->set_label(_("Similar images"));
		} else
		{
			m_pLabelSearchMode->set_label(_("Exact duplicates"));
		}
	}
}


}}
