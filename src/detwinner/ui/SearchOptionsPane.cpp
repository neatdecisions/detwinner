/*
 ===============================================================================
 Name        : SearchOptionsPane.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/SearchOptionsPane.hpp>

#include <ui/SearchSettingsDialog.hpp>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
SearchOptionsPane::SearchOptionsPane() :
	Gtk::Box(Gtk::ORIENTATION_VERTICAL),
	m_refActionGroup(Gio::SimpleActionGroup::create()),
	m_searchSettingsManager(Glib::build_filename(Glib::get_user_config_dir(), "com.neatdecisions.detwinner", "settings.ini"))
{
	m_searchSettingsManager.loadSettings();

	m_refActionGroup->add_action("selectAll", sigc::mem_fun(*this, &SearchOptionsPane::on_select_all_clicked));
	m_refActionGroup->add_action("clearSelection", sigc::mem_fun(*this, &SearchOptionsPane::on_clear_selection_clicked));
	m_refActionGroup->add_action("reload", sigc::mem_fun(*this, &SearchOptionsPane::on_refresh_clicked));
	m_refActionGroup->add_action("searchSettingsExactDuplicates", sigc::mem_fun(*this, &SearchOptionsPane::on_search_settings_exact_duplicates_clicked));
	m_refActionGroup->add_action("searchSettingsSimilarImages", sigc::mem_fun(*this, &SearchOptionsPane::on_search_settings_similar_images_clicked));
	m_refActionGroup->add_action("searchSettingsDefault", sigc::mem_fun(*this, &SearchOptionsPane::on_search_settings_default_clicked));

	m_refActionShowHidden = m_refActionGroup->add_action_bool("showHiddenFiles", sigc::mem_fun(*this, &SearchOptionsPane::on_show_hidden_toggled));
	m_refActionSearchMode = m_refActionGroup->add_action_radio_integer("searchMode", sigc::mem_fun(*this, &SearchOptionsPane::on_search_mode_changed), 0);

	m_places.set_local_only(false);
	m_places.set_show_trash(false);
	m_places.set_show_enter_location(false);
	m_places.set_show_other_locations(false);
	// set to true and then to false - to ensure that update_places is executed
	m_places.set_show_recent(true);
	m_places.set_show_recent(false);
	m_places.signal_open_location().connect(sigc::mem_fun(*this, &SearchOptionsPane::on_open_location));

	m_scrolledWindow.add(m_fileTree);
	m_scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	auto pMainBox = Gtk::manage(new Gtk::Paned());
	pMainBox->add(m_places);
	pMainBox->add(m_scrolledWindow);
	pack_end(*pMainBox);
	const std::string & aHomeDir = Glib::get_home_dir();
	m_places.set_location(Gio::File::create_for_path(aHomeDir));
	m_fileTree.load(aHomeDir);
	on_search_mode_changed(getIntBySearchMode(m_searchSettingsManager.getSearchSettings().searchMode));

	const bool showHiddenFiles = m_searchSettingsManager.getSearchSettings().uiSettings.showHiddenFiles;
	if (m_refActionShowHidden) m_refActionShowHidden->change_state(showHiddenFiles);
	m_fileTree.set_show_hidden(showHiddenFiles);
}


//------------------------------------------------------------------------------
SearchOptionsPane::signal_search_mode_changed_t
SearchOptionsPane::signal_search_mode_changed()
{
	return m_signalSearchModeChanged;
}


//------------------------------------------------------------------------------
settings::SearchSettings
SearchOptionsPane::getSearchSettings() const
{
	return m_searchSettingsManager.getSearchSettings();
}


//------------------------------------------------------------------------------
std::vector<std::string>
SearchOptionsPane::getSearchPaths() const
{
	return m_fileTree.fetchSelectedPaths();
}


//------------------------------------------------------------------------------
Glib::RefPtr<Gio::SimpleActionGroup>
SearchOptionsPane::getActionGroup()
{
	return m_refActionGroup;
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_search_mode_changed(int value)
{
	m_refActionSearchMode->change_state(value);
	settings::SearchSettings searchSettings = m_searchSettingsManager.getSearchSettings();
	searchSettings.searchMode = getSearchModeByInt(value);
	m_searchSettingsManager.setSearchSettings(searchSettings);
	m_searchSettingsManager.saveSettings();
	m_signalSearchModeChanged.emit(getSearchModeByInt(value));
}


//------------------------------------------------------------------------------
settings::SearchSettings::SearchMode_t
SearchOptionsPane::getSearchModeByInt(int value) const
{
	return (value == 1) ?
			settings::SearchSettings::SearchMode_t::kSimilarImages :
			settings::SearchSettings::SearchMode_t::kExactDuplicates;
}


//------------------------------------------------------------------------------
int
SearchOptionsPane::getIntBySearchMode(settings::SearchSettings::SearchMode_t value) const
{
	return (value == settings::SearchSettings::SearchMode_t::kSimilarImages) ? 1 : 0;
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_open_location(const Glib::RefPtr<Gio::File> & location, Gtk::PlacesOpenFlags)
{
	if (location)
	{
		m_places.set_location(location);
		m_fileTree.load(location->get_path());
	}
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_select_all_clicked()
{
	m_fileTree.selectAll();
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_clear_selection_clicked()
{
	m_fileTree.clearSelection();
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_refresh_clicked()
{
	m_fileTree.refresh();
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_show_hidden_toggled()
{
	bool toggle = false;
	if (m_refActionShowHidden)
	{
		m_refActionShowHidden->get_state(toggle);
		toggle = !toggle;
		m_refActionShowHidden->change_state(toggle);
	}
	auto searchSettings = m_searchSettingsManager.getSearchSettings();
	searchSettings.uiSettings.showHiddenFiles = toggle;
	m_searchSettingsManager.setSearchSettings(searchSettings);
	m_searchSettingsManager.saveSettings();
	m_fileTree.set_show_hidden(toggle);
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::show_search_settings_dialog(settings::SearchSettings::SearchMode_t mode)
{
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_resource("/com/neatdecisions/detwinner/ui/settingsDialog.ui");
	SearchSettingsDialog * tempPtr = nullptr;
	builder->get_widget_derived("SettingsDialog", tempPtr);
	g_assert_nonnull(tempPtr);
	std::unique_ptr<SearchSettingsDialog> pDialog(tempPtr);
	pDialog->init(mode, m_searchSettingsManager.getSearchSettings());
	Gtk::Widget * pParent = get_toplevel();
	if (pParent && pParent->get_is_toplevel())
	{
		pDialog->set_transient_for(dynamic_cast<Gtk::Window&>(*pParent));
	}
	if (pDialog->run() == Gtk::RESPONSE_OK)
	{
		m_searchSettingsManager.setSearchSettings(pDialog->getSettings());
		m_searchSettingsManager.saveSettings();
	}
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_search_settings_similar_images_clicked()
{
	show_search_settings_dialog(settings::SearchSettings::SearchMode_t::kSimilarImages);
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_search_settings_exact_duplicates_clicked()
{
	show_search_settings_dialog(settings::SearchSettings::SearchMode_t::kExactDuplicates);
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::on_search_settings_default_clicked()
{
	show_search_settings_dialog(m_searchSettingsManager.getSearchSettings().searchMode);
}

}}
