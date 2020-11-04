/*
 ===============================================================================
 Name        : SearchSettingsDialog.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/SearchSettingsDialog.hpp>

#include <glibmm/i18n.h>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
SearchSettingsDialog::SearchSettingsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
		Gtk::Dialog(cobject),
		m_builder(builder)
{
	const std::map<Glib::ustring, CommonWidgets&> widgetMap = {
		{"ED", m_widgetsExactDuplicates},
		{"SI", m_widgetsSimilarImages}
	};
	m_builder->get_widget("stackSwitcher", m_stackSwitcher);
	m_builder->get_widget("btnOK", m_btnOk);
	m_builder->get_widget("btnCancel", m_btnCancel);
	m_btnOk->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &SearchSettingsDialog::on_response), Gtk::RESPONSE_OK));
	m_btnCancel->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &SearchSettingsDialog::on_response), Gtk::RESPONSE_CANCEL));

	m_builder->get_widget("spinSimilarity", m_spinSimilarity);
	m_builder->get_widget("chkRotations", m_checkRotations);

	for (auto & kv: widgetMap) {
		m_builder->get_widget("chkGreaterThan" + kv.first, kv.second.m_checkMinFileSize);
		m_builder->get_widget("chkSmallerThan" + kv.first, kv.second.m_checkMaxFileSize);
		m_builder->get_widget("spinGreaterThan" + kv.first, kv.second.m_spinMinFileSize);
		m_builder->get_widget("spinSmallerThan" + kv.first, kv.second.m_spinMaxFileSize);
		m_builder->get_widget("comboGreaterThan" + kv.first, kv.second.m_comboMinFileSize);
		m_builder->get_widget("comboSmallerThan" + kv.first, kv.second.m_comboMaxFileSize);
		m_builder->get_widget("chkReadOnly" + kv.first, kv.second.m_checkReadOnlyFiles);
		m_builder->get_widget("chkHidden" + kv.first, kv.second.m_checkHiddenFiles);
		m_builder->get_widget("chkExecutable" + kv.first, kv.second.m_checkExecutableFiles);
		m_builder->get_widget("lblRegex" + kv.first, kv.second.m_labelRegexps);
		m_builder->get_widget("btnAddRegex" + kv.first, kv.second.m_btnAddRegex);
		m_builder->get_widget_derived("listRegex" + kv.first, kv.second.m_listboxRegex);
		setupWidgets(kv.second);
	}
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::setupWidgets(CommonWidgets & widgets)
{
	widgets.m_listboxRegex->on_input_status_changed().connect(
		sigc::bind<CommonWidgets&>(sigc::mem_fun(*this, &SearchSettingsDialog::on_regex_listbox_changed), sigc::ref(widgets)));
	widgets.m_btnAddRegex->signal_clicked().connect(
		sigc::bind<CommonWidgets&>(sigc::mem_fun(*this, &SearchSettingsDialog::on_add_regex_line_clicked), sigc::ref(widgets)));

	widgets.m_checkMinFileSize->signal_toggled().connect(
		sigc::bind<CommonWidgets&>(sigc::mem_fun(*this, &SearchSettingsDialog::on_filesize_min_toggled), sigc::ref(widgets)));

	widgets.m_checkMaxFileSize->signal_toggled().connect(
		sigc::bind<CommonWidgets&>(sigc::mem_fun(*this, &SearchSettingsDialog::on_filesize_max_toggled), sigc::ref(widgets)));
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::init(
		settings::SearchSettings::SearchMode_t mode,
		const settings::SearchSettings & exactDuplicatesSettings,
		const settings::SearchSettings & similarImagesSettings)
{
	m_stackSwitcher->get_stack()->set_visible_child(
		mode == settings::SearchSettings::SearchMode_t::kSimilarImages ? "pageSimilarImages" : "pageExactDuplicates");
	populateCommonWidgets(exactDuplicatesSettings, m_widgetsExactDuplicates);
	populateCommonWidgets(similarImagesSettings, m_widgetsSimilarImages);
	m_spinSimilarity->set_value(similarImagesSettings.imageSettings->sensitivity);
	m_checkRotations->set_active(similarImagesSettings.imageSettings->processRotations);
	show_all_children();
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_filesize_min_toggled(CommonWidgets & widgets)
{
	widgets.m_spinMinFileSize->set_sensitive(widgets.m_checkMinFileSize->get_active());
	widgets.m_comboMinFileSize->set_sensitive(widgets.m_checkMinFileSize->get_active());
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_filesize_max_toggled(CommonWidgets & widgets)
{
	widgets.m_spinMaxFileSize->set_sensitive(widgets.m_checkMaxFileSize->get_active());
	widgets.m_comboMaxFileSize->set_sensitive(widgets.m_checkMaxFileSize->get_active());
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_add_regex_line_clicked(CommonWidgets & widgets)
{
	widgets.m_listboxRegex->addLine();
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::on_regex_listbox_changed(CommonWidgets & widgets)
{
	m_btnOk->set_sensitive(widgets.m_listboxRegex->isInputValid());
	m_stackSwitcher->set_sensitive(widgets.m_listboxRegex->isInputValid());
	widgets.m_labelRegexps->set_text(Glib::ustring::compose(_("<b>File path regexps</b> (%1 in total)"), widgets.m_listboxRegex->get_children().size()));
	widgets.m_labelRegexps->set_use_markup(true);
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
SearchSettingsDialog::populateCommonWidgets(const settings::SearchSettings & settings, CommonWidgets & widgets)
{
	if (settings.minFileSize)
	{
		widgets.m_checkMinFileSize->set_active(settings.minFileSize->enabled);
		widgets.m_spinMinFileSize->set_sensitive(settings.minFileSize->enabled);
		widgets.m_comboMinFileSize->set_sensitive(settings.minFileSize->enabled);
		widgets.m_spinMinFileSize->set_value(settings.minFileSize->size);
		setUnitComboboxValue(settings.minFileSize->unit, *widgets.m_comboMinFileSize);
	} else
	{
		widgets.m_spinMinFileSize->set_sensitive(false);
		widgets.m_comboMinFileSize->set_sensitive(false);
	}

	if (settings.maxFileSize)
	{
		widgets.m_checkMaxFileSize->set_active(settings.maxFileSize->enabled);
		widgets.m_spinMaxFileSize->set_sensitive(settings.maxFileSize->enabled);
		widgets.m_comboMaxFileSize->set_sensitive(settings.maxFileSize->enabled);
		widgets.m_spinMaxFileSize->set_value(settings.maxFileSize->size);
		setUnitComboboxValue(settings.maxFileSize->unit, *widgets.m_comboMaxFileSize);
	} else
	{
		widgets.m_spinMaxFileSize->set_sensitive(false);
		widgets.m_comboMaxFileSize->set_sensitive(false);
	}

	widgets.m_checkReadOnlyFiles->set_active(settings.searchReadOnly);
	widgets.m_checkHiddenFiles->set_active(settings.searchHidden);
	widgets.m_checkExecutableFiles->set_active(settings.searchExecutable);

	widgets.m_listboxRegex->populateRegexps(settings.filenameRegexps);
	on_regex_listbox_changed(widgets);
}


//------------------------------------------------------------------------------
void
SearchSettingsDialog::populateCommonSettings(const CommonWidgets & widgets,
		settings::SearchSettings & settings) const
{
	settings.minFileSize = settings::SearchSettings::FileSizeSetting_t();
	settings.minFileSize->enabled = widgets.m_checkMinFileSize->get_active();
	settings.minFileSize->size = widgets.m_spinMinFileSize->get_value_as_int();
	settings.minFileSize->unit = getUnitComboboxValue(*widgets.m_comboMinFileSize);

	settings.maxFileSize = settings::SearchSettings::FileSizeSetting_t();
	settings.maxFileSize->enabled = widgets.m_checkMaxFileSize->get_active();
	settings.maxFileSize->size = widgets.m_spinMaxFileSize->get_value_as_int();
	settings.maxFileSize->unit = getUnitComboboxValue(*widgets.m_comboMaxFileSize);

	settings.searchReadOnly = widgets.m_checkReadOnlyFiles->get_active();
	settings.searchHidden = widgets.m_checkHiddenFiles->get_active();
	settings.searchExecutable = widgets.m_checkExecutableFiles->get_active();

	settings.filenameRegexps = widgets.m_listboxRegex->getRegexps();
}


//------------------------------------------------------------------------------
settings::SearchSettings
SearchSettingsDialog::getSettings(settings::SearchSettings::SearchMode_t mode) const
{
	settings::SearchSettings settings;
	settings.searchMode = mode;
	if (settings.searchMode == settings::SearchSettings::SearchMode_t::kSimilarImages)
	{
		settings.imageSettings = settings::SearchSettings::ImageSettings_t();
		settings.imageSettings->sensitivity = m_spinSimilarity->get_value();
		settings.imageSettings->processRotations = m_checkRotations->get_active();
		populateCommonSettings(m_widgetsSimilarImages, settings);
	} else
	{
		populateCommonSettings(m_widgetsExactDuplicates, settings);
	}
	return settings;
}


}}
