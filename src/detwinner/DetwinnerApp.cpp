/*
 ===============================================================================
 Name        : DetwinnerApp.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <DetwinnerApp.hpp>

#include <logic/Initializer.hpp>
#include <ui/DetwinnerWindow.hpp>

#include <glibmm/i18n.h>
#include <config.h>


namespace detwinner {


//------------------------------------------------------------------------------
DetwinnerApp::DetwinnerApp() : Gtk::Application("com.neatdecisions.Detwinner")
{
	Glib::set_application_name(_("Detwinner"));
}


//------------------------------------------------------------------------------
std::unique_ptr<DetwinnerApp>
DetwinnerApp::Create()
{
	return std::unique_ptr<DetwinnerApp>(new DetwinnerApp());
}


//------------------------------------------------------------------------------
void
DetwinnerApp::on_startup()
{
	Gtk::Application::on_startup();

  add_action("about", sigc::mem_fun(*this, &DetwinnerApp::on_action_about));
  add_action("quit", sigc::mem_fun(*this, &DetwinnerApp::on_action_quit));
  set_accel_for_action("app.quit", "<Ctrl>Q");
	logic::Initialize();
}


//------------------------------------------------------------------------------
void
DetwinnerApp::on_activate()
{
	create_window();
}


//------------------------------------------------------------------------------
void
DetwinnerApp::create_window()
{
	auto win = new ui::DetwinnerWindow();

	win->set_default_icon_name("com.neatdecisions.Detwinner");

	//Make sure that the application runs for as long this window is still open:
	add_window(*win);

	//Delete the window when it is hidden.
	//That's enough for this simple example.
	win->signal_hide().connect(sigc::bind<Gtk::Window*>(
			sigc::mem_fun(*this, &DetwinnerApp::on_window_hide), win));

	win->set_position(Gtk::WIN_POS_CENTER);
	win->present();
}


//------------------------------------------------------------------------------
void
DetwinnerApp::on_window_hide(Gtk::Window * window)
{
	delete window;
}


//------------------------------------------------------------------------------
void
DetwinnerApp::on_action_quit()
{
	for (auto && window : get_windows())
	{
		if (window) window->hide();
	}
	quit();
}


//------------------------------------------------------------------------------
void
DetwinnerApp::on_action_about()
{
	Gtk::AboutDialog aboutDialog;
	aboutDialog.set_program_name(_("Detwinner"));
	aboutDialog.set_version(PACKAGE_VERSION);
	aboutDialog.set_comments(_("Neat duplicate file finder"));
	aboutDialog.set_website("https://neatdecisions.com");
	aboutDialog.set_website_label("neatdecisions.com");
	aboutDialog.set_license_type(Gtk::LICENSE_GPL_3_0);
	aboutDialog.set_copyright("© 2018–2020 Neat Decisions");
	aboutDialog.set_logo_icon_name("com.neatdecisions.Detwinner");
	aboutDialog.set_translator_credits(_("translator-credits"));
	aboutDialog.set_modal(true);

	auto parent = get_active_window();

	if (parent)
	{
		aboutDialog.set_transient_for(*parent);
		aboutDialog.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	} else
	{
		aboutDialog.set_position(Gtk::WIN_POS_CENTER_ALWAYS);
	}
	aboutDialog.run();
}


}
