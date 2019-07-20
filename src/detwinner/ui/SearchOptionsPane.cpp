/*
 ===============================================================================
 Name        : SearchOptionsPane.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
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

	m_refActionShowHidden = m_refActionGroup->add_action_bool("showHiddenFiles", sigc::mem_fun(*this, &SearchOptionsPane::on_show_hidden_toggled));
	m_refActionSearchMode = m_refActionGroup->add_action_radio_integer("searchMode", sigc::mem_fun(*this, &SearchOptionsPane::on_search_mode_changed), 0);

	m_places.set_local_only(true);
	m_places.set_show_trash(false);
	m_places.set_show_enter_location(false);
	m_places.set_show_other_locations(false);
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
	on_search_mode_changed(getIntBySearchMode(m_searchSettingsManager.getDefaultMode()));
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
	int state = 0;
	m_refActionSearchMode->get_state(state);
	return m_searchSettingsManager.getSearchSettings(getSearchModeByInt(state));
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
	settings::SearchSettings::SearchMode_t searchMode = getSearchModeByInt(value);
	m_searchSettingsManager.setDefaultMode(searchMode);
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
	m_fileTree.set_show_hidden(toggle);
}


//------------------------------------------------------------------------------
void
SearchOptionsPane::show_search_settings_dialog(settings::SearchSettings::SearchMode_t mode)
{
	Gtk::Widget * pParent = get_toplevel();
	if (pParent && pParent->get_is_toplevel())
	{
		SearchSettingsDialog settingsDialog(
				dynamic_cast<Gtk::Window&>(*pParent),
				m_searchSettingsManager.getSearchSettings(mode));

		if (settingsDialog.run() == Gtk::RESPONSE_OK)
		{
			m_searchSettingsManager.setSearchSettings(settingsDialog.getSettings());
			m_searchSettingsManager.saveSettings();
		}
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


}}
