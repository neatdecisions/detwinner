/*
 ===============================================================================
 Name        : SearchSettingsDialog.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef UI_SEARCHSETTINGSDIALOG_HPP_
#define UI_SEARCHSETTINGSDIALOG_HPP_


#include <gtkmm.h>
#include <settings/SearchSettings.hpp>
#include <ui/RegexListbox.hpp>


namespace detwinner {
namespace ui {


class SearchSettingsDialog : public Gtk::Dialog
{
public:
	SearchSettingsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	settings::SearchSettings getSettings() const;
	void init(settings::SearchSettings::SearchMode_t mode, const settings::SearchSettings & searchSettings);

private:
	struct CommonWidgets
	{
		Gtk::CheckButton * m_checkMinFileSize = nullptr;
		Gtk::CheckButton * m_checkMaxFileSize = nullptr;
		Gtk::SpinButton * m_spinMinFileSize = nullptr;
		Gtk::SpinButton * m_spinMaxFileSize = nullptr;

		Gtk::ComboBoxText * m_comboMinFileSize = nullptr;
		Gtk::ComboBoxText * m_comboMaxFileSize = nullptr;

		Gtk::CheckButton * m_checkReadOnlyFiles = nullptr;
		Gtk::CheckButton * m_checkHiddenFiles = nullptr;
		Gtk::CheckButton * m_checkExecutableFiles = nullptr;
		Gtk::Label * m_labelRegexps = nullptr;
		RegexListbox * m_listboxRegex = nullptr;
		Gtk::Button * m_btnAddRegex = nullptr;
	};

	void on_filesize_min_toggled(CommonWidgets & widgets);
	void on_filesize_max_toggled(CommonWidgets & widgets);
	void on_add_regex_line_clicked(CommonWidgets & widgets);
	void on_regex_listbox_changed(CommonWidgets & widgets);

	void populateCommonWidgets(const settings::SearchSettings::CommonSettings & settings, CommonWidgets & widgets);
	void populateCommonSettings(const CommonWidgets & widgets, settings::SearchSettings::CommonSettings & settings) const;

	void setUnitComboboxValue(const settings::SearchSettings::FileSizeUnit_t value, Gtk::ComboBoxText & combobox);
	settings::SearchSettings::FileSizeUnit_t getUnitComboboxValue(const Gtk::ComboBoxText & combobox) const;

	void setupWidgets(CommonWidgets & widgets);

	Glib::RefPtr<Gtk::Builder> m_builder;
	settings::SearchSettings::SearchMode_t m_searchMode = settings::SearchSettings::SearchMode_t::kExactDuplicates;

	Gtk::StackSwitcher * m_stackSwitcher = nullptr;
	Gtk::SpinButton * m_spinSimilarity = nullptr;
	Gtk::CheckButton * m_checkRotations = nullptr;
	CommonWidgets m_widgetsExactDuplicates;
	CommonWidgets m_widgetsSimilarImages;

	Gtk::Button * m_btnOk = nullptr;
	Gtk::Button * m_btnCancel = nullptr;
};


}}

#endif /* UI_SEARCHSETTINGSDIALOG_HPP_ */
