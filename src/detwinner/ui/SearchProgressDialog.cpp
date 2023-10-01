/*
 ===============================================================================
 Name        : SearchProgressDialog.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/SearchProgressDialog.hpp>

#include <glibmm.h>
#include <glibmm/i18n.h>

#include <logic/DuplicateFilesFinder.hpp>
#include <logic/DuplicateImageFinder.hpp>

namespace detwinner::ui {

//------------------------------------------------------------------------------
SearchProgressDialog::SearchProgressDialog(Gtk::Window & parent,
                                           const settings::SearchSettings & searchSettings,
                                           const std::vector<std::string> & pathsToSearch,
                                           const callbacks::IDuplicateReceiver::Ptr & duplicateReceiver)
		: Gtk::Dialog(_("Search progress"), parent, true), m_duplicateReceiver(duplicateReceiver), m_btnStop(_("Stop")),
			m_btnPause(_("Pause")), m_labelStage("", Gtk::ALIGN_START, Gtk::ALIGN_BASELINE),
			m_labelTotalProcessed("", Gtk::ALIGN_START, Gtk::ALIGN_BASELINE),
			m_labelSkipped("", Gtk::ALIGN_START, Gtk::ALIGN_BASELINE),
			m_labelDuplicatesFound("", Gtk::ALIGN_START, Gtk::ALIGN_BASELINE),
			m_labelTimeSpent("", Gtk::ALIGN_START, Gtk::ALIGN_BASELINE), m_searchMode(searchSettings.searchMode),
			m_progressItem(callbacks::SearchProgressCallback::Create())
{
	m_progressBar.set_show_text(false);
	m_progressBar.set_text(_("Paused"));

	m_grid.set_border_width(1);
	m_grid.set_margin_bottom(6);
	m_grid.set_margin_top(6);
	m_grid.set_margin_start(6);
	m_grid.set_margin_end(6);
	m_grid.set_column_spacing(12);
	m_grid.set_row_spacing(6);

	const std::vector<Gtk::Label *> labels = {
			Gtk::make_managed<Gtk::Label>(_("<b>Stage:</b> "), Gtk::ALIGN_END, Gtk::ALIGN_BASELINE),
			Gtk::make_managed<Gtk::Label>(_("<b>Total files processed:</b> "), Gtk::ALIGN_END, Gtk::ALIGN_BASELINE),
			Gtk::make_managed<Gtk::Label>(_("<b>Skipped files:</b> "), Gtk::ALIGN_END, Gtk::ALIGN_BASELINE),
			Gtk::make_managed<Gtk::Label>(_("<b>Duplicates found:</b> "), Gtk::ALIGN_END, Gtk::ALIGN_BASELINE),
			Gtk::make_managed<Gtk::Label>(_("<b>Time spent:</b> "), Gtk::ALIGN_END, Gtk::ALIGN_BASELINE)};

	unsigned int rowNum = 0;
	for (Gtk::Label * pLabel : labels)
	{
		pLabel->set_use_markup(true);
		m_grid.attach(*pLabel, 0, rowNum++, 1, 1);
	}

	m_grid.attach(m_labelStage, 1, 0, 1, 1);
	m_grid.attach(m_labelTotalProcessed, 1, 1, 1, 1);
	m_grid.attach(m_labelSkipped, 1, 2, 1, 1);
	m_grid.attach(m_labelDuplicatesFound, 1, 3, 1, 1);
	m_grid.attach(m_labelTimeSpent, 1, 4, 1, 1);

	get_content_area()->pack_start(m_grid);
	get_content_area()->pack_start(m_progressBar, Gtk::PACK_SHRINK);

	auto buttonBox = Gtk::make_managed<Gtk::ButtonBox>(Gtk::ORIENTATION_HORIZONTAL);

	buttonBox->pack_end(m_btnPause);
	buttonBox->pack_end(m_btnStop);
	buttonBox->set_layout(Gtk::BUTTONBOX_CENTER);
	get_content_area()->pack_end(*buttonBox, Gtk::PACK_SHRINK);

	m_btnPause.signal_clicked().connect(sigc::mem_fun(*this, &SearchProgressDialog::on_pause_clicked));
	m_btnStop.signal_clicked().connect(sigc::mem_fun(*this, &SearchProgressDialog::on_stop_clicked));

	set_size_request(400, -1);
	set_resizable(false);
	show_all_children();

	m_progressItem->accessDispatcher().connect(sigc::mem_fun(*this, &SearchProgressDialog::switch_to_finish));
	m_refreshConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &SearchProgressDialog::on_timeout), 1000);

	// convert SearchSettings to FileSearchSettings
	const settings::SearchSettings::CommonSettings & commonSettings =
			(m_searchMode == settings::SearchSettings::SearchMode::SimilarImages) ? searchSettings.similarImagesSettings
																																						: searchSettings.exactDuplicatesSettings;
	logic::FileSearchSettings fileSearchSettings;
	fileSearchSettings.searchReadOnlyFiles = commonSettings.searchReadOnly;
	fileSearchSettings.searchHiddenFiles = commonSettings.searchHidden;
	fileSearchSettings.searchExecutableFiles = commonSettings.searchExecutable;
	fileSearchSettings.filenameRegexps = commonSettings.filenameRegexps;
	if (commonSettings.minFileSize && commonSettings.minFileSize->enabled)
	{
		fileSearchSettings.minFileSize =
				commonSettings.minFileSize->size *
				(std::pow(10ULL, static_cast<unsigned int>(commonSettings.minFileSize->unit) * 3ULL));
	}

	if (commonSettings.maxFileSize && commonSettings.maxFileSize->enabled)
	{
		fileSearchSettings.maxFileSize =
				commonSettings.maxFileSize->size *
				(std::pow(10ULL, static_cast<unsigned int>(commonSettings.maxFileSize->unit) * 3ULL));
	}

	if (m_searchMode == settings::SearchSettings::SearchMode::SimilarImages)
	{
		fileSearchSettings.sensitivity = searchSettings.imageSettings.sensitivity;
		fileSearchSettings.processRotations = searchSettings.imageSettings.processRotations;
		auto threadLambda = [pathsToSearch, fileSearchSettings, this]() {
			return logic::DuplicateImageFinder().find(pathsToSearch, fileSearchSettings, m_progressItem);
		};
		m_result = std::async(std::launch::async, threadLambda);
	} else
	{
		auto threadLambda = [pathsToSearch, fileSearchSettings, this]() {
			return logic::DuplicateFilesFinder().find(pathsToSearch, fileSearchSettings, m_progressItem);
		};
		m_result = std::async(std::launch::async, threadLambda);
	}
}

//------------------------------------------------------------------------------
SearchProgressDialog::~SearchProgressDialog() noexcept { m_refreshConnection.disconnect(); }

//------------------------------------------------------------------------------
void
SearchProgressDialog::update_indicators()
{
	if (m_searchMode == settings::SearchSettings::SearchMode::SimilarImages)
	{
		switch (m_progressItem->getStage())
		{
		case 0:
			m_labelStage.set_text(_("(1/4) Indexing…"));
			break;
		case 1:
			m_labelStage.set_text(_("(2/4) Examining images…"));
			break;
		case 2:
			m_labelStage.set_text(_("(3/4) Comparing images…"));
			break;
		case 3:
			m_labelStage.set_text(_("(4/4) Organizing images…"));
			break;
		default:
			break;
		}
	} else
	{
		switch (m_progressItem->getStage())
		{
		case 0:
			m_labelStage.set_text(_("(1/2) Indexing…"));
			break;
		case 1:
			m_labelStage.set_text(_("(2/2) Comparing…"));
			break;
		default:
			break;
		}
	}

	m_labelTotalProcessed.set_text(
			Glib::ustring::compose(_("%1 of %2"), m_progressItem->getProcessedCount(), m_progressItem->getTotalCount()));
	m_labelSkipped.set_text(Glib::ustring::compose("%1", m_progressItem->getSkippedCount()));

	m_labelDuplicatesFound.set_text(Glib::ustring::compose(
			_("%1 in %2 groups (total size: %3, can be freed: %4)"), m_progressItem->getDuplicateCount(),
			m_progressItem->getGroupCount(), Glib::format_size(m_progressItem->getDuplicatesSize()),
			Glib::format_size(m_progressItem->getWastedSize())));
	m_labelTimeSpent.set_text(
			Glib::DateTime::create_now_utc(static_cast<gint64>(m_progressItem->getElapsedTime())).format("%H∶%M∶%S"));

	if ((m_progressItem->getStage() == 0) || (m_progressItem->getTotalCount() == 0))
	{
		if (!m_progressItem->isPaused())
		{
			m_progressBar.pulse();
		}
	} else
	{
		const double progress =
				static_cast<double>(m_progressItem->getProcessedCount()) / static_cast<double>(m_progressItem->getTotalCount());
		m_progressBar.set_fraction(progress);
	}
}

//------------------------------------------------------------------------------
bool
SearchProgressDialog::on_timeout()
{
	update_indicators();
	return !m_progressItem->isFinished();
}

//------------------------------------------------------------------------------
void
SearchProgressDialog::on_pause_clicked()
{
	m_progressItem->togglePause();
	if (m_progressItem->isPaused())
	{
		m_btnPause.set_label(_("Continue"));
		m_progressBar.set_show_text(true);
	} else
	{
		m_btnPause.set_label(_("Pause"));
		m_progressBar.set_show_text(false);
	}
}

//------------------------------------------------------------------------------
void
SearchProgressDialog::on_stop_clicked()
{
	if (m_switchedToFinish || m_progressItem->isFinished())
	{
		m_isPopulationInterrupted = true;
		response(Gtk::RESPONSE_NONE);
	} else
	{
		m_progressItem->stop();
		switch_to_finish();
	}
}

//------------------------------------------------------------------------------
void
SearchProgressDialog::switch_to_finish()
{
	m_switchedToFinish = true;
	m_refreshConnection.disconnect();
	update_indicators();

	m_labelStage.set_text(_("Preparing results…"));

	m_progressBar.set_fraction(0.0);
	m_btnPause.hide();
	m_btnStop.set_label(_("Abort"));

	bool atLeastOneAdded = false;
	if (m_result.valid() && m_duplicateReceiver)
	{
		callbacks::IDeferredAction::Ptr pPopulateAction = m_duplicateReceiver->populate(m_result.get());
		if (pPopulateAction)
		{
			while (pPopulateAction->processNext())
			{
				atLeastOneAdded = true;
				m_progressBar.set_fraction(pPopulateAction->getProgress());
				Gtk::Main::iteration(false);
				if (m_isPopulationInterrupted)
				{
					response(Gtk::RESPONSE_OK);
					return;
				}
			}
		}
	}

	m_progressBar.set_fraction(1.0);
	m_labelStage.set_text(_("Finished."));
	m_btnStop.set_label(_("Close"));

	if (atLeastOneAdded)
	{
		response(Gtk::RESPONSE_OK);
	}
}

//------------------------------------------------------------------------------
void
SearchProgressDialog::on_response(int response_id)
{
	m_refreshConnection.disconnect();
	m_progressItem->stop();
	Gtk::Dialog::on_response(response_id);
}

} // namespace detwinner::ui
