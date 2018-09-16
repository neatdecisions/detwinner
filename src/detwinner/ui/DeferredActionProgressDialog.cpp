/*
 ===============================================================================
 Name        : DeferredActionProgressDialog.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/DeferredActionProgressDialog.hpp>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
DeferredActionProgressDialog::DeferredActionProgressDialog(
	const Glib::ustring & title,
	const callbacks::IDeferredAction::Ptr_t & action) :
		m_action(action), m_cancelled(false)
{
	init(title);
}


//------------------------------------------------------------------------------
DeferredActionProgressDialog::DeferredActionProgressDialog(
	const Glib::ustring & title,
	const callbacks::IDeferredAction::Ptr_t & action,
	Gtk::Window & parent) :
		Gtk::Dialog("", parent, true),
		m_action(action),
		m_cancelled(false)
{
	init(title);
}


//------------------------------------------------------------------------------
void
DeferredActionProgressDialog::init(const Glib::ustring & title)
{
	Gtk::Box * pBox = Gtk::manage(new Gtk::Box());

	Gtk::Button * pBtn = Gtk::manage(new Gtk::Button());
	pBtn->set_image_from_icon_name("process-stop", Gtk::ICON_SIZE_BUTTON);
	pBtn->set_relief(Gtk::RELIEF_NONE);
	pBtn->set_always_show_image(true);
	pBtn->set_can_focus(false);
	pBtn->signal_clicked().connect( sigc::mem_fun(*this, &DeferredActionProgressDialog::on_cancel_clicked) );
	pBtn->set_valign(Gtk::ALIGN_CENTER);

	m_progressBar.set_show_text(true);
	m_progressBar.set_text(title);
	m_progressBar.set_valign(Gtk::ALIGN_CENTER);
	m_progressBar.set_size_request(80, -1);

	pBox->pack_start(m_progressBar);
	pBox->pack_end(*pBtn, Gtk::PACK_SHRINK);

	get_content_area()->pack_start(*pBox, Gtk::PACK_SHRINK);

	signal_show().connect( sigc::mem_fun(*this, &DeferredActionProgressDialog::on_show_progress) );
	//set_decorated(false);
	set_resizable(false);
	set_size_request(200, -1);
}


//------------------------------------------------------------------------------
void
DeferredActionProgressDialog::on_cancel_clicked()
{
	m_cancelled = true;
}


//------------------------------------------------------------------------------
void
DeferredActionProgressDialog::on_show_progress()
{
	show_all_children();

	set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	if (m_action)
	{
		while (m_action->processNext())
		{
			m_progressBar.set_fraction(m_action->getProgress());
			Gtk::Main::iteration(false);
			if (m_cancelled) break;
		}
	}

	m_progressBar.set_fraction(1.0);
	close();
}


}}
