/*
===============================================================================
Name        : SearchProgressDialog.hpp
Author      : NeatDecisions
Version     :
Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
Description : Detwinner
===============================================================================
*/

#pragma once

#include <future>

#include <gtkmm.h>

#include <callbacks/IDuplicateReceiver.hpp>
#include <callbacks/SearchProgressCallback.hpp>
#include <logic/CommonDataTypes.hpp>
#include <settings/SearchSettings.hpp>

namespace detwinner::ui {

class SearchProgressDialog : public Gtk::Dialog
{
public:
	SearchProgressDialog(Gtk::Window & parent,
	                     const settings::SearchSettings & searchSettings,
	                     const std::vector<std::string> & pathsToSearch,
	                     const callbacks::IDuplicateReceiver::Ptr & duplicateReceiver);
	virtual ~SearchProgressDialog() noexcept override;

	bool on_timeout();
	void on_pause_clicked();
	void on_stop_clicked();

protected:
	virtual void on_response(int response_id) override;

private:
	void switch_to_finish();
	void update_indicators();

	callbacks::IDuplicateReceiver::Ptr m_duplicateReceiver;
	Gtk::Button m_btnStop;
	Gtk::Button m_btnPause;

	Gtk::ProgressBar m_progressBar;

	Gtk::Grid m_grid;

	Gtk::Label m_labelStage;
	Gtk::Label m_labelTotalProcessed;
	Gtk::Label m_labelSkipped;
	Gtk::Label m_labelDuplicatesFound;
	Gtk::Label m_labelTimeSpent;
	sigc::connection m_refreshConnection;

	const settings::SearchSettings::SearchMode m_searchMode;
	callbacks::SearchProgressCallback::Ptr m_progressItem;

	std::future<logic::DuplicatesList> m_result;
	bool m_switchedToFinish = false;
	bool m_isPopulationInterrupted = false;
};

} // namespace detwinner::ui
