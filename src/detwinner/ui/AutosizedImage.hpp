/*
 ===============================================================================
 Name        : AutosizedImage.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>

namespace detwinner::ui {

class AutosizedImage : public Gtk::Image
{
public:
	void set(const std::string & filename);
	void clear();

protected:
	virtual Gtk::SizeRequestMode get_request_mode_vfunc() const override;
	virtual void on_size_allocate(Gtk::Allocation & allocation) override;
	virtual void get_preferred_width_vfunc(int & minimum_width, int & natural_width) const override;
	virtual void get_preferred_height_vfunc(int & minimum_height, int & natural_height) const override;

private:
	Glib::RefPtr<Gdk::Pixbuf> m_basePixBuf;
};

} // namespace detwinner::ui
