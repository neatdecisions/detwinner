/*
 ===============================================================================
 Name        : RegexListbox.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/RegexListbox.hpp>
#include <gtkmm.h>
#include <glibmm/i18n.h>

#include <regex>


namespace detwinner {
namespace ui {


//==============================================================================
// RegexListbox
//==============================================================================

//------------------------------------------------------------------------------
RegexListbox::RegexListbox()
{
	set_selection_mode(Gtk::SELECTION_NONE);
	set_can_focus(true); // for GtkEntry to be able to get rid of focus on Enter
}


//------------------------------------------------------------------------------
RegexListbox::signal_input_status_changed
RegexListbox::on_input_status_changed()
{
	return m_signalStatusChanged;
}


//------------------------------------------------------------------------------
void
RegexListbox::addLine()
{
	auto pNewRow = Gtk::manage(new RegexRow("", *this));
	insert(*pNewRow, 0);
	show_all_children();
	pNewRow->grab_edit();
	m_signalStatusChanged.emit();
}


//------------------------------------------------------------------------------
bool
RegexListbox::isInputValid() const
{
	for (auto pWidget : get_children())
	{
		auto pRow = dynamic_cast<const RegexRow *>(pWidget);
		if (!pRow) continue;
		const std::string & rxString = pRow->getRegex();
		if (rxString.empty()) return false;
		try
		{
			const std::regex rx(rxString);
		} catch (const std::regex_error & ex)
		{
			return false;
		}

	}
	return true;
}


//------------------------------------------------------------------------------
std::vector<std::string>
RegexListbox::getRegexps() const
{
	std::vector<std::string> result;
	result.reserve(get_children().size());
	for (auto pWidget : get_children())
	{
		auto pRow = dynamic_cast<const RegexRow *>(pWidget);
		if (!pRow) continue;
		const std::string & rx = pRow->getRegex();
		if (!rx.empty()) result.push_back(rx);
	}
	return result;
}


//------------------------------------------------------------------------------
void
RegexListbox::populateRegexps(const std::vector<std::string> & regexps)
{
	while (auto pRow = this->get_row_at_index(0))
	{
		remove(*pRow);
	}

	for (const std::string & regex: regexps)
	{
		add(*Gtk::manage(new RegexRow(regex, *this)));
	}
}



//==============================================================================
// RegexListbox::RegexRow
//==============================================================================

//------------------------------------------------------------------------------
RegexListbox::RegexRow::RegexRow(const std::string & regex, RegexListbox & regexListbox) :
	m_regexListbox(regexListbox),
	m_pButton(Gtk::manage(new Gtk::Button(""))),
	m_pEntry(Gtk::manage(Gtk::manage(new Gtk::Entry()))),
	m_refActionGroup(Gio::SimpleActionGroup::create())
{
	static auto cssProvider = []() {
		auto css = Gtk::CssProvider::create();
		css->load_from_data("entry#noborder {border: none;}");
		return css;
	}();

	auto m_refBuilder = Gtk::Builder::create();
	m_refBuilder->add_from_resource("/com/neatdecisions/detwinner/ui/regexHelperMenu.ui");
	Glib::RefPtr<Glib::Object> object = m_refBuilder->get_object("menu-regex-helper");
	Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
	m_refMenu = std::make_unique<Gtk::Menu>(gmenu);
	if (!m_refMenu->get_attach_widget())
	{
		m_refMenu->attach_to_widget(*this);
	}

	m_refActionGroup->add_action("files_with_name", sigc::mem_fun(*this, &RegexListbox::RegexRow::on_menu_files_with_name));
	m_refActionGroup->add_action("files_in_folder", sigc::mem_fun(*this, &RegexListbox::RegexRow::on_menu_files_in_folder));
	m_refActionGroup->add_action("files_with_extension", sigc::mem_fun(*this, &RegexListbox::RegexRow::on_menu_files_with_extension));
	insert_action_group("regexlistbox", m_refActionGroup);

	m_pEntry->set_name("noborder");

	auto pStyleCtx = m_pEntry->get_style_context();
	if (pStyleCtx) pStyleCtx->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	Gtk::Box * pBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
	m_pButton->set_always_show_image(true);

	try
	{
		const std::regex rx(regex);
		m_pEntry->set_text(regex);
	} catch (const std::regex_error & ex)
	{
		g_warning("%s", ex.what());
		m_pEntry->set_text("");
	}

	m_pEntry->set_icon_from_icon_name("view-more-symbolic");
	m_pEntry->set_has_frame(false);
	m_pEntry->signal_changed().connect(sigc::mem_fun(*this, &RegexListbox::RegexRow::on_line_changed));
	m_pEntry->signal_focus_in_event().connect(sigc::mem_fun(*this, &RegexListbox::RegexRow::on_focus_in));
	m_pEntry->signal_focus_out_event().connect(sigc::mem_fun(*this, &RegexListbox::RegexRow::on_focus_out));
	m_pEntry->signal_icon_press().connect(sigc::mem_fun(*this, &RegexListbox::RegexRow::on_icon_press));
	m_pEntry->signal_activate().connect(sigc::mem_fun(regexListbox, &RegexListbox::grab_focus));
	m_pButton->set_image_from_icon_name("list-remove-symbolic");
	m_pButton->set_relief(Gtk::RELIEF_NONE);
	m_pButton->set_always_show_image(true);
	m_pButton->signal_clicked().connect(sigc::mem_fun(*this, &RegexListbox::RegexRow::on_remove_line_clicked));
	validate();
	pBox->pack_start(*m_pEntry, Gtk::PACK_EXPAND_WIDGET);
	pBox->pack_end(*m_pButton, Gtk::PACK_SHRINK);
	add(*pBox);
	set_border_width(0);
}


//------------------------------------------------------------------------------
RegexListbox::RegexRow::~RegexRow() noexcept
{
	if (m_refMenu && m_refMenu->get_attach_widget()) m_refMenu->detach();
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::validate()
{
	bool isValid = !m_pEntry->get_text().empty();

	if (isValid)
	{
		try
		{
			const std::regex reg(m_pEntry->get_text().c_str());
		} catch (const std::regex_error &)
		{
			isValid = false;
		}
	}
	if (isValid)
	{
		m_pEntry->unset_icon(Gtk::ENTRY_ICON_SECONDARY);
	} else
	{
		m_pEntry->set_icon_from_icon_name("dialog-warning", Gtk::ENTRY_ICON_SECONDARY);
		m_pEntry->set_icon_tooltip_text(_("Invalid regexp"), Gtk::ENTRY_ICON_SECONDARY);
	}
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::on_line_changed()
{
	validate();
	m_regexListbox.m_signalStatusChanged.emit();
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::on_remove_line_clicked()
{
	m_regexListbox.remove(*this);
	m_regexListbox.m_signalStatusChanged.emit();
}


//------------------------------------------------------------------------------
bool
RegexListbox::RegexRow::on_focus_out(GdkEventFocus *)
{
	if (m_pEntry) m_pEntry->set_name("noborder");
	return true;
}


//------------------------------------------------------------------------------
bool
RegexListbox::RegexRow::on_focus_in(GdkEventFocus *)
{
	if (m_pEntry) m_pEntry->set_name("");
	return true;
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::on_icon_press(Gtk::EntryIconPosition iconPosition, const GdkEventButton * event)
{
	if ( (iconPosition == Gtk::ENTRY_ICON_PRIMARY) && m_refMenu && event )
	{
		m_refMenu->popup_at_pointer(reinterpret_cast<const GdkEvent*>(event));
	}
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::on_menu_files_with_name()
{
	if (!m_pEntry) return;
	m_pEntry->grab_focus();
	m_pEntry->set_text(".*?/FILENAME(\\.[^.]*$|$)");
	m_pEntry->select_region(4, 12);
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::on_menu_files_in_folder()
{
	if (!m_pEntry) return;
	m_pEntry->grab_focus();
	m_pEntry->set_text("^FOLDER/[^/]+$");
	m_pEntry->select_region(1, 7);
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::on_menu_files_with_extension()
{
	if (!m_pEntry) return;
	m_pEntry->grab_focus();
	m_pEntry->set_text(".*?\\.EXT$");
	m_pEntry->select_region(5, 8);
}


//------------------------------------------------------------------------------
std::string
RegexListbox::RegexRow::getRegex() const
{
	return m_pEntry->get_text();
}


//------------------------------------------------------------------------------
void
RegexListbox::RegexRow::grab_edit()
{
	m_pEntry->grab_focus();
}


}}
