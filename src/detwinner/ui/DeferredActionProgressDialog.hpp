/*
 ===============================================================================
 Name        : DeferredActionProgressDialog.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>

#include <callbacks/IDeferredAction.hpp>

namespace detwinner::ui {

class DeferredActionProgressDialog : public Gtk::Dialog
{
public:
	DeferredActionProgressDialog(const Glib::ustring & title, const callbacks::IDeferredAction::Ptr & action);

	DeferredActionProgressDialog(const Glib::ustring & title,
	                             const callbacks::IDeferredAction::Ptr & action,
	                             Gtk::Window & parent);

private:
	void init(const Glib::ustring & title);
	void on_show_progress();
	void on_cancel_clicked();
	callbacks::IDeferredAction::Ptr m_action;
	Gtk::ProgressBar m_progressBar;
	bool m_cancelled = false;
};

} // namespace detwinner::ui
