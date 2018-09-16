/*
 ===============================================================================
 Name        : DeferredActionProgressDialog.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef UI_DEFERREDACTIONPROGRESSDIALOG_HPP_
#define UI_DEFERREDACTIONPROGRESSDIALOG_HPP_

#include <gtkmm.h>
#include <callbacks/IDeferredAction.hpp>


namespace detwinner {
namespace ui {


class DeferredActionProgressDialog: public Gtk::Dialog
{
public:
	DeferredActionProgressDialog(
		const Glib::ustring & title,
		const callbacks::IDeferredAction::Ptr_t & action);

	DeferredActionProgressDialog(
		const Glib::ustring & title,
		const callbacks::IDeferredAction::Ptr_t & action,
		Gtk::Window& parent);

private:
	void init(const Glib::ustring & title);
	void on_show_progress();
	void on_cancel_clicked();
	callbacks::IDeferredAction::Ptr_t m_action;
	Gtk::ProgressBar m_progressBar;
	bool m_cancelled;
};


}}

#endif /* UI_DEFERREDACTIONPROGRESSDIALOG_HPP_ */
