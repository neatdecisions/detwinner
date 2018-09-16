/*
 ===============================================================================
 Name        : SearchSettingsDialog.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
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
	SearchSettingsDialog(Gtk::Window & parent, const settings::SearchSettings & settings);
	settings::SearchSettings getSettings() const;

protected:
	virtual void on_response(int response_id) override;

private:
	void on_filesize_min_toggled();
	void on_filesize_max_toggled();
	void on_add_regex_line_clicked();
	void on_regex_listbox_changed();

	void populateViewFromModel();
	void populateModelFromView();

	void configureGrid(Gtk::Grid & grid);

	void setUnitComboboxValue(const settings::SearchSettings::FileSizeUnit_t value, Gtk::ComboBoxText & combobox);
	settings::SearchSettings::FileSizeUnit_t getUnitComboboxValue(const Gtk::ComboBoxText & combobox) const;

	settings::SearchSettings m_settings;

	Gtk::Label m_labelSimilarImages;
	Gtk::Label m_labelSimilarity;
	Gtk::SpinButton m_spinSimilarity;
	Gtk::CheckButton m_checkRotations;

	Gtk::Label m_labelFileSize;
	Gtk::CheckButton m_checkMinFileSize;
	Gtk::CheckButton m_checkMaxFileSize;
	Gtk::SpinButton m_spinMinFileSize;
	Gtk::SpinButton m_spinMaxFileSize;

	Gtk::Label m_labelFileAttributes;
	Gtk::ComboBoxText m_comboMinFileSize;
	Gtk::ComboBoxText m_comboMaxFileSize;

	Gtk::CheckButton m_checkReadOnlyFiles;
	Gtk::CheckButton m_checkHiddenFiles;
	Gtk::CheckButton m_checkExecutableFiles;

	Gtk::Grid m_grid1;
	Gtk::Grid m_grid2;
	Gtk::Grid m_grid3;
	Gtk::Grid m_grid4;

	Gtk::Label m_labelRegexps;
	RegexListbox m_listboxRegex;

	Gtk::Button * m_btnOk;

	Gtk::ScrolledWindow m_scrolledWindow;
};


}}

#endif /* UI_SEARCHSETTINGSDIALOG_HPP_ */
