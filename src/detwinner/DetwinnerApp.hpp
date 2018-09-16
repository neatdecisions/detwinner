/*
 ===============================================================================
 Name        : DetwinnerApp.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */


#ifndef DETWINNERAPP_HPP_
#define DETWINNERAPP_HPP_

#include <gtkmm.h>


namespace detwinner {


class DetwinnerApp : public Gtk::Application
{
public:
	static std::unique_ptr<DetwinnerApp> create();

protected:
	DetwinnerApp();
	void on_startup() override;
	void on_activate() override;

private:
	void create_window();
	void on_window_hide(Gtk::Window * window);
	void on_action_about();
	void on_action_quit();

	Glib::RefPtr<Gtk::Builder> m_refBuilder;
};


}

#endif /* DETWINNERAPP_HPP_ */
