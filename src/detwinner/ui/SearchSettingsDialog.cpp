/*
 ===============================================================================
 Name        : SearchSettingsDialog.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/SearchSettingsDialog.hpp>

#include <glibmm/i18n.h>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
SearchSettingsDialog::SearchSettingsDialog(Gtk::Window & parent, const settings::SearchSettings & settings) :
		Gtk::Dialog(_("Settings"), parent, true),
		m_settings(settings),
		m_labelSimilarImages(_("<b>Similar images</b>"), Gtk::ALIGN_START),
		m_labelSimilarity(_("Similarity level:")),
		m_checkRotations(_("Take rotations into account")),
		m_labelFileSize(_("<b>File size</b>"), Gtk::ALIGN_START),
		m_checkMinFileSize(_("Greater than:")),
		m_checkMaxFileSize(_("Smaller than:")),
		m_labelFileAttributes(_("<b>File attributes</b>"), Gtk::ALIGN_START),
		m_checkReadOnlyFiles(_("Read only")),
		m_checkHiddenFiles(_("Hidden")),
		m_checkExecutableFiles(_("Executable")),
		m_labelRegexps(_("<b>File path regexps</b>"), Gtk::ALIGN_START),
		m_btnOk(add_button(_("OK"), Gtk::RESPONSE_OK))
{
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	get_content_area()->set_border_width(12);
	get_content_area()->set_spacing(6);

	configureGrid(m_grid1);
	configureGrid(m_grid2);
	configureGrid(m_grid3);
	configureGrid(m_grid4);

	if (m_settings.searchMode != settings::SearchSettings::SearchMode_t::kSimilarImages)
	{
		m_settings.imageSettings = stdx::nullopt;
	}

	if (m_settings.imageSettings)
	{
		m_labelSimilarImages.set_use_markup(true);
		m_grid1.attach(m_labelSimilarImages, 0, 0, 2, 1);

		m_spinSimilarity.set_range(1, 100);
		m_spinSimilarity.set_increments(1, 10);
		m_spinSimilarity.set_halign(Gtk::ALIGN_END);

		m_grid1.attach(m_labelSimilarity, 0, 1, 1, 1);
		m_grid1.attach(m_spinSimilarity, 1, 1, 1, 1);

		m_labelSimilarity.set_margin_start(6);
		m_labelSimilarity.set_halign(Gtk::ALIGN_START);

		m_checkRotations.set_margin_start(6);
		m_grid1.attach(m_checkRotations, 0, 2, 2, 1);

		get_content_area()->pack_start(m_grid1, Gtk::PACK_SHRINK);
	} else
	{
		m_labelSimilarity.hide();
		m_spinSimilarity.hide();
		m_checkRotations.hide();
	}

	for (auto && pSpinner: std::vector<Gtk::SpinButton*>{&m_spinMinFileSize, &m_spinMaxFileSize})
	{
		pSpinner->set_increments(1, 100);
		pSpinner->set_range(0, 1000000);
	}

	for (auto && pCombo: std::vector<Gtk::ComboBoxText*>{&m_comboMinFileSize, &m_comboMaxFileSize})
	{
		pCombo->append(_("B"));
		pCombo->append(_("KB"));
		pCombo->append(_("MB"));
		pCombo->append(_("GB"));
		pCombo->set_active_text(_("B"));
	}

	m_labelFileSize.set_use_markup(true);
	m_grid2.attach(m_labelFileSize, 0, 0, 3, 1);

	m_checkMinFileSize.set_margin_start(6);
	m_spinMinFileSize.set_halign(Gtk::ALIGN_END);
	m_grid2.attach(m_checkMinFileSize, 0, 1, 1, 1);
	m_grid2.attach(m_spinMinFileSize, 1, 1, 1, 1);
	m_grid2.attach(m_comboMinFileSize, 2, 1, 1, 1);

	m_checkMaxFileSize.set_margin_start(6);
	m_spinMaxFileSize.set_halign(Gtk::ALIGN_END);
	m_grid2.attach(m_checkMaxFileSize, 0, 2, 1, 1);
	m_grid2.attach(m_spinMaxFileSize, 1, 2, 1, 1);
	m_grid2.attach(m_comboMaxFileSize, 2, 2, 1, 1);

	m_labelFileAttributes.set_use_markup(true);
	m_grid3.attach(m_labelFileAttributes, 0, 0, 2, 1);
	m_checkReadOnlyFiles.set_margin_start(6);
	m_checkHiddenFiles.set_margin_start(6);
	m_checkExecutableFiles.set_margin_start(6);
	m_grid3.attach(m_checkReadOnlyFiles, 0, 1, 1, 1);
	m_grid3.attach(m_checkHiddenFiles, 0, 2, 1, 1);
	m_grid3.attach(m_checkExecutableFiles, 0, 3, 1, 1);

	m_checkMinFileSize.signal_toggled().connect(sigc::mem_fun(*this, &SearchSettingsDialog::on_filesize_min_toggled));
	m_checkMaxFileSize.signal_toggled().connect(sigc::mem_fun(*this, &SearchSettingsDialog::on_filesize_max_toggled));

	get_content_area()->pack_start(m_grid2, Gtk::PACK_SHRINK);
	get_content_area()->pack_start(m_grid3, Gtk::PACK_SHRINK);

	m_listboxRegex.on_input_status_changed().connect(sigc::mem_fun(*this, &SearchSettingsDialog::on_regex_listbox_changed));
	auto pButton = Gtk::manage(new Gtk::Button(""));
	pButton->signal_clicked().connect(sigc::mem_fun(*this, &SearchSettingsDialog::on_add_regex_line_clicked));
	pButton->set_halign(Gtk::ALIGN_END);
	pButton->set_image_from_icon_name("list-add-symbolic");
	pButton->set_relief(Gtk::RELIEF_NONE);
	pButton->set_always_show_image(true);
	m_labelRegexps.set_use_markup(true);
	m_listboxRegex.set_hexpand(true);
	m_grid4.attach(m_labelRegexps, 0, 0, 2, 1);
	m_grid4.attach(*pButton, 2, 0, 1, 1);


	m_scrolledWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	m_scrolledWindow.set_min_content_height(100);
	m_scrolledWindow.add(m_listboxRegex);
	m_scrolledWindow.set_shadow_type(Gtk::SHADOW_IN);

	m_grid4.attach(m_scrolledWindow, 0, 1, 3, 1);

	get_content_area()->pack_end(m_grid4, Gtk::PACK_EXPAND_WIDGET);

	populateViewFromModel();
	on_regex_listbox_changed();
	show_all_children();
	resize(1, 1);
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::configureGrid(Gtk::Grid & grid)
{
	grid.set_border_width(6);
	grid.set_margin_bottom(0);
	grid.set_margin_top(0);
	grid.set_margin_start(0);
	grid.set_margin_end(0);
	grid.set_column_spacing(12);
	grid.set_row_spacing(6);
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_filesize_min_toggled()
{
	m_spinMinFileSize.set_sensitive(m_checkMinFileSize.get_active());
	m_comboMinFileSize.set_sensitive(m_checkMinFileSize.get_active());
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_filesize_max_toggled()
{
	m_spinMaxFileSize.set_sensitive(m_checkMaxFileSize.get_active());
	m_comboMaxFileSize.set_sensitive(m_checkMaxFileSize.get_active());
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_add_regex_line_clicked()
{
	m_listboxRegex.addLine();
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_regex_listbox_changed()
{
	m_btnOk->set_sensitive(m_listboxRegex.isInputValid());
	m_labelRegexps.set_text(Glib::ustring::compose(_("<b>File path regexps</b> (%1 in total)"), m_listboxRegex.get_children().size()));
	m_labelRegexps.set_use_markup(true);
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_response(int response_id)
{
	if (response_id == Gtk::RESPONSE_OK)
	{
		populateModelFromView();
	}
	Gtk::Dialog::on_response(response_id);
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::setUnitComboboxValue(
		const settings::SearchSettings::FileSizeUnit_t value, Gtk::ComboBoxText & combobox)
{
	switch (value)
	{
		case settings::SearchSettings::FileSizeUnit_t::kGB:
			combobox.set_active_text(_("GB"));
			break;
		case settings::SearchSettings::FileSizeUnit_t::kMB:
			combobox.set_active_text(_("MB"));
			break;
		case settings::SearchSettings::FileSizeUnit_t::kKB:
			combobox.set_active_text(_("KB"));
			break;
		default:
			combobox.set_active_text(_("B"));
			break;
	}
}


//------------------------------------------------------------------------------
settings::SearchSettings::FileSizeUnit_t
SearchSettingsDialog::getUnitComboboxValue(const Gtk::ComboBoxText & combobox) const
{
	const Glib::ustring & text = combobox.get_active_text();
	if (text == _("GB")) return settings::SearchSettings::FileSizeUnit_t::kGB;
	if (text == _("MB")) return settings::SearchSettings::FileSizeUnit_t::kMB;
	if (text == _("KB")) return settings::SearchSettings::FileSizeUnit_t::kKB;
	return settings::SearchSettings::FileSizeUnit_t::kB;
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::populateViewFromModel()
{
	if (m_settings.imageSettings)
	{
		m_spinSimilarity.set_value(m_settings.imageSettings->sensitivity);
		m_checkRotations.set_active(m_settings.imageSettings->processRotations);
	}

	if (m_settings.minFileSize)
	{
		m_checkMinFileSize.set_active(m_settings.minFileSize->enabled);
		m_spinMinFileSize.set_sensitive(m_settings.minFileSize->enabled);
		m_comboMinFileSize.set_sensitive(m_settings.minFileSize->enabled);
		m_spinMinFileSize.set_value(m_settings.minFileSize->size);
		setUnitComboboxValue(m_settings.minFileSize->unit, m_comboMinFileSize);
	} else
	{
		m_spinMinFileSize.set_sensitive(false);
		m_comboMinFileSize.set_sensitive(false);
	}

	if (m_settings.maxFileSize)
	{
		m_checkMaxFileSize.set_active(m_settings.maxFileSize->enabled);
		m_spinMaxFileSize.set_sensitive(m_settings.maxFileSize->enabled);
		m_comboMaxFileSize.set_sensitive(m_settings.maxFileSize->enabled);
		m_spinMaxFileSize.set_value(m_settings.maxFileSize->size);
		setUnitComboboxValue(m_settings.maxFileSize->unit, m_comboMaxFileSize);
	} else
	{
		m_spinMaxFileSize.set_sensitive(false);
		m_comboMaxFileSize.set_sensitive(false);
	}

	m_checkReadOnlyFiles.set_active(m_settings.searchReadOnly);
	m_checkHiddenFiles.set_active(m_settings.searchHidden);
	m_checkExecutableFiles.set_active(m_settings.searchExecutable);

	m_listboxRegex.populateRegexps(m_settings.filenameRegexps);
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::populateModelFromView()
{
	if (m_settings.imageSettings)
	{
		if (m_spinSimilarity.is_visible()) m_settings.imageSettings->sensitivity = m_spinSimilarity.get_value();
		if (m_checkRotations.is_visible()) m_settings.imageSettings->processRotations = m_checkRotations.get_active();
	}

	m_settings.minFileSize = settings::SearchSettings::FileSizeSetting_t();
	m_settings.minFileSize->enabled = m_checkMinFileSize.get_active();
	m_settings.minFileSize->size = m_spinMinFileSize.get_value_as_int();
	m_settings.minFileSize->unit = getUnitComboboxValue(m_comboMinFileSize);

	m_settings.maxFileSize = settings::SearchSettings::FileSizeSetting_t();
	m_settings.maxFileSize->enabled = m_checkMaxFileSize.get_active();
	m_settings.maxFileSize->size = m_spinMaxFileSize.get_value_as_int();
	m_settings.maxFileSize->unit = getUnitComboboxValue(m_comboMaxFileSize);

	m_settings.searchReadOnly = m_checkReadOnlyFiles.get_active();
	m_settings.searchHidden = m_checkHiddenFiles.get_active();
	m_settings.searchExecutable = m_checkExecutableFiles.get_active();

	m_settings.filenameRegexps = m_listboxRegex.getRegexps();
}


//------------------------------------------------------------------------------
settings::SearchSettings
SearchSettingsDialog::getSettings() const
{
	return m_settings;
}


}}
