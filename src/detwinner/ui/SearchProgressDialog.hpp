	/*
 ===============================================================================
 Name        : SearchProgressDialog.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef UI_SEARCHPROGRESSDIALOG_HPP_
#define UI_SEARCHPROGRESSDIALOG_HPP_

#include <gtkmm.h>
#include <callbacks/SearchProgressCallback.hpp>
#include <callbacks/IDuplicateReceiver.hpp>

#include <logic/CommonDataTypes.hpp>
#include <settings/SearchSettings.hpp>
#include <future>


namespace detwinner {
namespace ui {


class SearchProgressDialog : public Gtk::Dialog
{
public:
	SearchProgressDialog(
			Gtk::Window& parent,
			const settings::SearchSettings & searchSettings,
			const std::vector<std::string> & pathsToSearch,
			const callbacks::IDuplicateReceiver::Ptr_t & duplicateReceiver);
	virtual ~SearchProgressDialog() noexcept override;

	bool on_timeout();
	void on_pause_clicked();
	void on_stop_clicked();

protected:
	virtual void on_response(int response_id) override;

private:
	void switch_to_finish();
	void update_indicators();

	callbacks::IDuplicateReceiver::Ptr_t m_duplicateReceiver;
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

	const settings::SearchSettings m_searchSettings;
	callbacks::SearchProgressCallback::Ptr_t m_progressItem;

	std::future<logic::DuplicatesList_t> m_result;
	bool m_switchedToFinish;
	bool m_isPopulationInterrupted;
};


}}

#endif /* UI_SEARCHPROGRESSDIALOG_HPP_ */
