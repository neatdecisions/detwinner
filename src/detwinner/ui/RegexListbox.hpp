/*
 ===============================================================================
 Name        : RegexListbox.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2020 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef UI_REGEXLISTBOX_HPP_
#define UI_REGEXLISTBOX_HPP_

#include <gtkmm.h>


namespace detwinner {
namespace ui {


class RegexListbox : public Gtk::ListBox
{
public:
	RegexListbox(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
	std::vector<std::string> getRegexps() const;
	void populateRegexps(const std::vector<std::string> & iRegexps);
	void addLine();
	bool isInputValid() const;

	using signal_input_status_changed = sigc::signal<void>;
	signal_input_status_changed on_input_status_changed();

private:

	class RegexRow : public Gtk::ListBoxRow
	{
		public:
			RegexRow(const std::string & regex, RegexListbox & regexListbox);
			virtual ~RegexRow() noexcept override;
			std::string getRegex() const;
			void grab_edit();

		private:
			void validate();
			void on_remove_line_clicked();
			void on_line_changed();
			bool on_focus_out(GdkEventFocus *);
			bool on_focus_in(GdkEventFocus *);
			void on_icon_press(Gtk::EntryIconPosition iconPosition, const GdkEventButton *);
			void on_menu_files_with_name();
			void on_menu_files_in_folder();
			void on_menu_files_with_extension();

			RegexListbox & m_regexListbox;
			Gtk::Button * m_pButton;
			Gtk::Entry * m_pEntry;
			std::unique_ptr<Gtk::Menu> m_refMenu;
			Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;
	};

	signal_input_status_changed m_signalStatusChanged;
};


}}

#endif /* UI_REGEXLISTBOX_HPP_ */
