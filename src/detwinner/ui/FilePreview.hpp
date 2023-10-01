/*
 ===============================================================================
 Name        : FilePreview.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>

#include <ui/AutosizedImage.hpp>

namespace detwinner::ui {

class FilePreview : public Gtk::Box
{
public:
	FilePreview();
	void setFileName(const std::string & filePath);

private:
	void reload();
	Glib::ustring buildHexPreview(const char * buffer, unsigned long size) const;

	std::string m_filePath;
	Gtk::TextView m_previewText;
	AutosizedImage m_previewImage;
	Gtk::Stack m_previewStack;
	Gtk::ScrolledWindow m_scrolledWindowPreview;

	Gtk::Grid m_grid;
	Gtk::Label m_labelFileName;
	Gtk::Label m_labelFileSize;
	Gtk::Label m_labelFileModified;

	Gtk::Label m_labelFileName_value;
	Gtk::Label m_labelFileSize_value;
	Gtk::Label m_labelFileModified_value;
};

} // namespace detwinner::ui
