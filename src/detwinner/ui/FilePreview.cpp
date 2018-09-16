/*
 ===============================================================================
 Name        : FilePreview.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/FilePreview.hpp>

#include <glib.h>
#include <glib/gstdio.h>
#include <glibmm/i18n.h>
#include <iomanip>
#include <tools/MemoryMappedFile.hpp>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
FilePreview::FilePreview() :
	Gtk::Box(Gtk::ORIENTATION_VERTICAL),
	m_labelFileName(_("<b>File name:</b> "), Gtk::ALIGN_START, Gtk::ALIGN_BASELINE),
	m_labelFileSize(_("<b>File size:</b> "), Gtk::ALIGN_START, Gtk::ALIGN_BASELINE),
	m_labelFileModified(_("<b>Modified:</b> "), Gtk::ALIGN_START, Gtk::ALIGN_BASELINE)
{
	m_labelFileName.set_use_markup(true);
	m_labelFileSize.set_use_markup(true);
	m_labelFileModified.set_use_markup(true);

	m_grid.set_border_width(1);
	m_grid.set_margin_bottom(6);
	m_grid.set_margin_top(6);
	m_grid.set_margin_start(6);
	m_grid.set_margin_end(6);
	m_grid.set_column_spacing(12);
	m_grid.set_row_spacing(6);
	m_grid.attach(m_labelFileName, 0, 0, 1, 1);
	m_grid.attach(m_labelFileSize, 0, 1, 1, 1);
	m_grid.attach(m_labelFileModified, 0, 2, 1, 1);

	m_grid.attach(m_labelFileName_value, 1, 0, 1, 1);
	m_grid.attach(m_labelFileSize_value, 1, 1, 1, 1);
	m_grid.attach(m_labelFileModified_value, 1, 2, 1, 1);

	m_labelFileName_value.set_halign(Gtk::ALIGN_START);
	m_labelFileName_value.set_valign(Gtk::ALIGN_BASELINE);
	m_labelFileSize_value.set_halign(Gtk::ALIGN_START);
	m_labelFileSize_value.set_valign(Gtk::ALIGN_BASELINE);
	m_labelFileModified_value.set_halign(Gtk::ALIGN_START);
	m_labelFileModified_value.set_valign(Gtk::ALIGN_BASELINE);

	m_labelFileName_value.set_selectable(true);
	m_labelFileSize_value.set_selectable(true);
	m_labelFileModified_value.set_selectable(true);

	m_labelFileName_value.set_text("-");
	m_labelFileSize_value.set_text("-");
	m_labelFileModified_value.set_text("-");
	m_labelFileName_value.set_ellipsize(Pango::ELLIPSIZE_MIDDLE);
	m_labelFileSize_value.set_ellipsize(Pango::ELLIPSIZE_END);
	m_labelFileModified_value.set_ellipsize(Pango::ELLIPSIZE_END);

	m_previewText.set_monospace(true);
	m_scrolledWindowPreview.add(m_previewText);
	m_scrolledWindowPreview.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_previewText.set_editable(false);
	m_previewStack.add(m_scrolledWindowPreview, "text");
	m_previewStack.add(m_previewImage, "image");

	pack_start(m_grid, Gtk::PACK_SHRINK);
	pack_end(m_previewStack, Gtk::PACK_EXPAND_WIDGET);
	set_homogeneous(false);
}


//------------------------------------------------------------------------------
void
FilePreview::setFile(const std::string & filePath)
{
	if (!filePath.empty())
	{
		if (m_filePath == filePath) return;
		m_labelFileName_value.set_text(filePath);

		const Glib::RefPtr<Gio::File> & file = Gio::File::create_for_path(filePath);
		if (file)
		{
			const Glib::RefPtr<Gio::FileInfo> & fileInfo = file->query_info("time::modified,standard::size");
			if (fileInfo)
			{
				m_labelFileSize_value.set_text(Glib::format_size(fileInfo->get_size()));

				if (fileInfo->has_attribute(G_FILE_ATTRIBUTE_TIME_MODIFIED))
				{
					const Glib::DateTime & aDateTime = Glib::DateTime::create_now_local(fileInfo->get_attribute_uint64(G_FILE_ATTRIBUTE_TIME_MODIFIED));
					m_labelFileModified_value.set_text(aDateTime.format("%x %X"));
				}
			}
		}
	} else
	{
		m_labelFileName_value.set_text("-");
		m_labelFileSize_value.set_text("-");
		m_labelFileModified_value.set_text("-");
	}

	m_filePath = filePath;

	reload();
}


//------------------------------------------------------------------------------
Glib::ustring
FilePreview::buildHexPreview(const char * buffer, unsigned long size) const
{
	Glib::ustring result;
	result.reserve(size * 3 + size + (size / 16 + ( (size % 16) ? 1 : 0)) * 10);

	for (unsigned long i = 0; i < size; i += 16)
	{
		unsigned long n = std::min(i + 16, size);

		result.append(Glib::ustring::format(std::hex, std::setfill(L'0'), std::setw(8), i ));
		result.append("\t");

		for (unsigned long j = i; j < n; ++j)
		{
			result.append(Glib::ustring::format(std::hex, std::setfill(L'0'), std::setw(2), static_cast<unsigned short>(static_cast<unsigned char>(buffer[j])) ));
			result.append(" ");
		}

		for (unsigned long j = n; j < i + 16; ++j)
		{
			result.append("   ");
		}
		result.append("\t");

		for (unsigned long j = i; j < n; ++j)
		{
			char c = buffer[j];
			if (c < 32 || c == 127) c = '.';
			result.append(&c, 1);
		}

		result.append("\n");
	}
	return result;
}


//------------------------------------------------------------------------------
void FilePreview::reload()
{
	m_previewText.get_buffer().clear();
	m_previewImage.clear();

	if (m_filePath.empty())
	{
		m_previewImage.set_from_icon_name("text-x-generic-symbolic", Gtk::ICON_SIZE_DIALOG);
		m_previewStack.set_visible_child("image");
		return;
	}
	bool uncertain;
	const Glib::ustring & fileType = Gio::content_type_guess(m_filePath, nullptr, 0, uncertain);

	if (fileType.substr(0, 5) == "image")
	{
		m_previewImage.set(m_filePath);
		m_previewStack.set_visible_child("image");
	} else
	{
		m_previewText.freeze_notify();
		m_previewText.get_buffer()->set_text(Glib::ustring());
		const tools::MemoryMappedFile memoryMappedFile(m_filePath);
		if (memoryMappedFile.valid())
		{
			const gsize dataSize = memoryMappedFile.size();
			if (dataSize > 0)
			{
				const gchar * pData = memoryMappedFile.buffer();
				const gchar * pValidDataEnd = pData;
				if (g_utf8_validate(pData, dataSize, &pValidDataEnd) == TRUE)
				{
					if (pData != pValidDataEnd)
					{
						m_previewText.get_buffer()->set_text(pData, pValidDataEnd);
					}
				} else
				{
					constexpr unsigned long kMaxSize = 1024;
					if (dataSize <= kMaxSize)
					{
						m_previewText.get_buffer()->set_text(buildHexPreview(pData, dataSize));
					} else
					{
						Glib::ustring cutContent = buildHexPreview(pData, kMaxSize);
						cutContent.append(Glib::ustring::compose(_("\n ... only first %1 shown ... \n"), Glib::format_size(kMaxSize, Glib::FORMAT_SIZE_LONG_FORMAT)));
						m_previewText.get_buffer()->set_text(cutContent);
					}
				}
			}
		}
		m_previewText.thaw_notify();
		m_previewStack.set_visible_child("text");
	}
}


}}
