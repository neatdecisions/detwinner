/*
 ===============================================================================
 Name        : SearchOptionsPane.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/SearchOptionsPane.hpp>

#include <ui/SearchSettingsDialog.hpp>

namespace detwinner::ui {

//------------------------------------------------------------------------------
SearchOptionsPane::SearchOptionsPane()
		: Gtk::Box(Gtk::ORIENTATION_VERTICAL), m_refActionGroup(Gio::SimpleActionGroup::create()),
			m_searchSettingsManager(
					Glib::build_filename(Glib::get_user_config_dir(), "com.neatdecisions.detwinner", "settings.ini"))
{
	m_searchSettingsManager.loadSettings();

	m_refActionGroup->add_action("selectAll", sigc::mem_fun(*this, &SearchOptionsPane::on_select_all_clicked));
	m_refActionGroup->add_action("clearSelection", sigc::mem_fun(*this, &SearchOptionsPane::on_clear_selection_clicked));
	m_refActionGroup->add_action("reload", sigc::mem_fun(*this, &SearchOptionsPane::on_refresh_clicked));
	m_refActionGroup->add_action("searchSettingsExactDuplicates",
	                             sigc::mem_fun(*this, &SearchOptionsPane::on_search_settings_exact_duplicates_clicked));
	m_refActionGroup->add_action("searchSettingsSimilarImages",
	                             sigc::mem_fun(*this, &SearchOptionsPane::on_search_settings_similar_images_clicked));
	m_refActionGroup->add_action("searchSettingsDefault",
	                             sigc::mem_fun(*this, &SearchOptionsPane::on_search_settings_default_clicked));

	m_refActionShowHidden = m_refActionGroup->add_action_bool(
			"showHiddenFiles", sigc::mem_fun(*this, &SearchOptionsPane::on_show_hidden_toggled));
	m_refActionSearchMode = m_refActionGroup->add_action_radio_integer(
			"searchMode", sigc::mem_fun(*this, &SearchOptionsPane::on_search_mode_changed),
			static_cast<int>(settings::SearchSettings::SearchMode::ExactDuplicates));

	m_places.signalOpenLocation().connect(sigc::mem_fun(*this, &SearchOptionsPane::on_open_location));

	m_scrolledWindow.add(m_fileTree);
	m_scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	auto pMainBox = Gtk::make_managed<Gtk::Paned>();
	pMainBox->add(m_places);
	pMainBox->add(m_scrolledWindow);
	pack_end(*pMainBox);

	on_open_location(m_places.getLocation());

	on_search_mode_changed(static_cast<int>(m_searchSettingsManager.getSearchSettings().searchMode));

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
SearchOptionsPane::on_search_mode_changed(int searchMode)
{
	m_refActionSearchMode->change_state(searchMode);
	settings::SearchSettings searchSettings = m_searchSettingsManager.getSearchSettings();
	searchSettings.searchMode = static_cast<settings::SearchSettings::SearchMode>(searchMode);
	m_searchSettingsManager.setSearchSettings(searchSettings);
	m_searchSettingsManager.saveSettings();
	m_signalSearchModeChanged.emit(static_cast<settings::SearchSettings::SearchMode>(searchMode));
}

//------------------------------------------------------------------------------
void
SearchOptionsPane::on_open_location(const Glib::RefPtr<const Gio::File> & location)
{
	if (location) m_fileTree.load(location->get_path());
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
SearchOptionsPane::show_search_settings_dialog(settings::SearchSettings::SearchMode mode)
{
	Glib::RefPtr<Gtk::Builder> builder =
			Gtk::Builder::create_from_resource("/com/neatdecisions/detwinner/ui/settingsDialog.ui");
	SearchSettingsDialog * tempPtr = nullptr;
	builder->get_widget_derived("SettingsDialog", tempPtr);
	g_assert_nonnull(tempPtr);
	std::unique_ptr<SearchSettingsDialog> pDialog(tempPtr);
	pDialog->init(mode, m_searchSettingsManager.getSearchSettings());
	Gtk::Widget * pParent = get_toplevel();
	if (pParent && pParent->get_is_toplevel())
	{
		pDialog->set_transient_for(dynamic_cast<Gtk::Window &>(*pParent));
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
	show_search_settings_dialog(settings::SearchSettings::SearchMode::SimilarImages);
}

//------------------------------------------------------------------------------
void
SearchOptionsPane::on_search_settings_exact_duplicates_clicked()
{
	show_search_settings_dialog(settings::SearchSettings::SearchMode::ExactDuplicates);
}

//------------------------------------------------------------------------------
void
SearchOptionsPane::on_search_settings_default_clicked()
{
	show_search_settings_dialog(m_searchSettingsManager.getSearchSettings().searchMode);
}

} // namespace detwinner::ui
