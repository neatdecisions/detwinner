/*
 ===============================================================================
 Name        : DetwinnerApp.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>

namespace detwinner {

class DetwinnerApp : public Gtk::Application
{
public:
	static std::unique_ptr<DetwinnerApp> Create();

protected:
	DetwinnerApp();
	void on_startup() override;
	void on_activate() override;

private:
	void create_window();
	void on_window_hide(Gtk::Window * window);
	void on_action_about();
	void on_action_quit();
};

} // namespace detwinner
