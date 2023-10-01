/*
 ===============================================================================
 Name        : AutosizedImage.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/AutosizedImage.hpp>

namespace detwinner::ui {

//------------------------------------------------------------------------------
void
AutosizedImage::clear()
{
	m_basePixBuf.clear();
	Gtk::Image::clear();
}

//------------------------------------------------------------------------------
void
AutosizedImage::set(const std::string & filename)
{
	auto pAnimation = Gdk::PixbufAnimation::create_from_file(filename);
	if (pAnimation && pAnimation->is_static_image())
	{
		m_basePixBuf = pAnimation->get_static_image();
	} else
	{
		m_basePixBuf.clear();
		Gtk::Image::set(pAnimation);
	}
}

//------------------------------------------------------------------------------
Gtk::SizeRequestMode
AutosizedImage::get_request_mode_vfunc() const
{
	return Gtk::SIZE_REQUEST_CONSTANT_SIZE;
}

//------------------------------------------------------------------------------
void
AutosizedImage::get_preferred_width_vfunc(int & minimum_width, int & natural_width) const
{
	minimum_width = 0;
	const Glib::RefPtr<const Gdk::Pixbuf> pPixBuf = get_pixbuf();
	natural_width = pPixBuf ? std::min(get_allocated_width(), pPixBuf->get_width()) : 0;
}

//------------------------------------------------------------------------------
void
AutosizedImage::get_preferred_height_vfunc(int & minimum_height, int & natural_height) const
{
	minimum_height = 0;
	const Glib::RefPtr<const Gdk::Pixbuf> pPixBuf = get_pixbuf();
	natural_height = pPixBuf ? pPixBuf->get_height() : 0;
}

//------------------------------------------------------------------------------
void
AutosizedImage::on_size_allocate(Gtk::Allocation & allocation)
{
	if (m_basePixBuf)
	{
		if ((m_basePixBuf->get_height() <= allocation.get_height()) &&
		    (m_basePixBuf->get_width() <= allocation.get_width()))
		{
			if (m_basePixBuf != get_pixbuf())
			{
				Gtk::Image::set(m_basePixBuf);
			}
		} else
		{
			const float kImageAspectRatio = static_cast<float>(m_basePixBuf->get_height()) / m_basePixBuf->get_width();
			const float kWindowAspectRatio = static_cast<float>(allocation.get_height()) / allocation.get_width();
			int height = 0;
			int width = 0;
			if (kImageAspectRatio < kWindowAspectRatio)
			{
				width = allocation.get_width();
				height = width * kImageAspectRatio;
			} else
			{
				height = allocation.get_height();
				width = height / kImageAspectRatio;
			}
			const Glib::RefPtr<const Gdk::Pixbuf> pExistingPixBuf = get_pixbuf();
			if (!pExistingPixBuf || (pExistingPixBuf->get_width() != width) || (pExistingPixBuf->get_height() != height))
			{
				Gtk::Image::set(m_basePixBuf->scale_simple(width, height, Gdk::INTERP_BILINEAR));
			}
		}
	}
	Gtk::Image::on_size_allocate(allocation);
}
} // namespace detwinner::ui
