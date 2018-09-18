/*
 ===============================================================================
 Name        : SearchResultsPane.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/SearchResultsPane.hpp>

#include <ui/DeferredActionProgressDialog.hpp>

#include <glibmm/i18n.h>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
SearchResultsPane::SearchResultsPane() :
	Gtk::Box(Gtk::ORIENTATION_VERTICAL),
	m_refActionGroup(Gio::SimpleActionGroup::create())
{
	m_refActionGroup->add_action("expandAll", sigc::mem_fun(*this, &SearchResultsPane::on_expand_all_clicked));
	m_refActionGroup->add_action("collapseAll", sigc::mem_fun(*this, &SearchResultsPane::on_collapse_all_clicked));

	m_refActionGroup->add_action("selectAll", sigc::mem_fun(*this, &SearchResultsPane::on_select_all));
	m_refActionGroup->add_action("selectNone", sigc::mem_fun(*this, &SearchResultsPane::on_select_none));
	m_refActionGroup->add_action("selectInvert", sigc::mem_fun(*this, &SearchResultsPane::on_select_invert));

	m_refActionGroup->add_action("selectKeepGroupFirst", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_first_in_group_clicked));
	m_refActionGroup->add_action("selectKeepGroupLast", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_last_in_group_clicked));
	m_refActionGroup->add_action("selectKeepDateEarliest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_earliest_modified_clicked));
	m_refActionGroup->add_action("selectKeepDateLatest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_latest_modified_clicked));
	m_refActionGroup->add_action("selectKeepNameShortest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_name_shortest_clicked));
	m_refActionGroup->add_action("selectKeepNameLongest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_name_longest_clicked));
	m_refActionGroup->add_action("selectKeepPathShortest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_path_shortest_clicked));
	m_refActionGroup->add_action("selectKeepPathLongest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_path_longest_clicked));

	m_refActionGroup->add_action("deletePermanently", sigc::mem_fun(*this, &SearchResultsPane::on_delete_permanently));
	// until flatpak can properly handle GIO trash
	#ifndef FLATPAK
	m_refActionGroup->add_action("deleteMoveToTrash", sigc::mem_fun(*this, &SearchResultsPane::on_delete_move_to_trash));
	#endif
	m_refActionGroup->add_action("deleteMoveToFolder", sigc::mem_fun(*this, &SearchResultsPane::on_delete_move_to_folder));

	m_refActionSortBy = m_refActionGroup->add_action_radio_integer("sortBy", sigc::mem_fun(*this, &SearchResultsPane::on_sort_by_changed), 0);
	m_refActionSortAscending = m_refActionGroup->add_action_bool("sortAscending", sigc::mem_fun(*this, &SearchResultsPane::on_sort_ascending_toggled));

	m_scrolledWindow.add(m_treeView);
	m_scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	m_treeView.on_duplicate_selected().connect( sigc::mem_fun(*this, &SearchResultsPane::on_duplicate_selected) );
	m_treeView.on_stats_changed().connect( sigc::mem_fun(*this, &SearchResultsPane::updateStatus) );

	m_previewPanel.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	m_previewPanel.pack1(m_filePreviewLeft, true, true);
	m_previewPanel.pack2(m_filePreviewRight, true, true);
	m_mainPanel.set_orientation(Gtk::ORIENTATION_VERTICAL);
	m_mainPanel.pack1(m_scrolledWindow, true, true);
	m_mainPanel.pack2(m_previewPanel, true, true);
	m_mainFrame.add(m_mainPanel);
	pack_start(m_mainFrame, true, true);
	pack_end(m_statusBar, false, false);

	setMode(settings::SearchSettings::SearchMode_t::kExactDuplicates);
}


//------------------------------------------------------------------------------
Glib::RefPtr<Gio::SimpleActionGroup>
SearchResultsPane::getActionGroup()
{
	return m_refActionGroup;
}


//------------------------------------------------------------------------------
callbacks::IDeferredAction::Result_t
SearchResultsPane::execute_duplicate_action(const Glib::ustring & label, callbacks::IDeferredAction::Ptr_t action)
{
	if (!action)
	{
		g_warning("SearchResultsPane: NULL action");
		return callbacks::IDeferredAction::Result_t::Unknown;
	}

	freeze_child_notify();
	freeze_notify();

	Glib::Timer timer;

	timer.start();
	while (action->processNext())
	{
		if (timer.elapsed() > 0.2)
		{
			timer.stop();
			Container * pContainer = get_toplevel();
			Gtk::Window * pWindow = dynamic_cast<Gtk::Window *>(pContainer);
			if (pWindow && pWindow->get_is_toplevel())
			{
				DeferredActionProgressDialog(label, action, *pWindow).run();
			} else
			{
				DeferredActionProgressDialog(label, action).run();
			}
			break;
		}
	}

	thaw_notify();
	thaw_child_notify();

	return action->getStatus();
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_first_in_group_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepFirstInGroup());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_last_in_group_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepLastInGroup());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_earliest_modified_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepEarliestModified());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_latest_modified_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepLatestModified());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_name_shortest_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepShortestName());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_name_longest_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepLongestName());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_path_shortest_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepShortestPath());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_path_longest_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepLongestPath());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_resolution_lowest_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepLowestResolution());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_smart_select_resolution_highest_clicked()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.smartSelect_KeepHighestResolution());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_select_all()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.selectAll());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_select_none()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.clearSelection());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_select_invert()
{
	execute_duplicate_action(_("Selecting…"), m_treeView.invertSelection());
}


//------------------------------------------------------------------------------
void
SearchResultsPane::sort_tree()
{
	m_treeView.freeze_notify();
	bool ascending = false;
	int type = 0;
	if (m_refActionSortAscending) m_refActionSortAscending->get_state(ascending);
	if (m_refActionSortBy) m_refActionSortBy->get_state(type);
	switch (type)
	{
	case 0:
		m_treeView.sortByTotalSize(ascending);
		break;
	case 1:
		m_treeView.sortBySingleFileSize(ascending);
		break;
	case 2:
		m_treeView.sortByNumberOfFiles(ascending);
		break;
	}
	m_treeView.thaw_notify();
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_sort_ascending_toggled()
{
	if (m_refActionSortAscending)
	{
		bool toggle = false;
		m_refActionSortAscending->get_state(toggle);
		toggle = !toggle;
		m_refActionSortAscending->change_state(toggle);
	}
	sort_tree();
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_sort_by_changed(int iValue)
{
	m_refActionSortBy->change_state(iValue);
	sort_tree();
}


//------------------------------------------------------------------------------
void
SearchResultsPane::init()
{
	m_treeView.clear();
	if (m_refActionSortBy) m_refActionSortBy->change_state(0);
	if (m_refActionSortAscending) m_refActionSortAscending->change_state(false);
	m_filePreviewLeft.setFile("");
	m_filePreviewRight.setFile("");
	m_previewPanel.remove(m_filePreviewRight);
}


//------------------------------------------------------------------------------
void
SearchResultsPane::updateStatus()
{
	const auto treeStats = m_treeView.calculateStats();
	m_statusBar.remove_all_messages();
	m_statusBar.push(Glib::ustring::compose(
			_("Remaining %1 duplicates in %2 groups (total size: %3, could be potentially freed: %4)"),
			treeStats.fileCount,
			treeStats.groupCount,
			Glib::format_size(treeStats.totalSize),
			Glib::format_size(treeStats.wastedSize)
	));
}


//------------------------------------------------------------------------------
void
SearchResultsPane::setMode(settings::SearchSettings::SearchMode_t iMode)
{
	if (iMode == settings::SearchSettings::SearchMode_t::kSimilarImages)
	{
		m_refActionGroup->add_action("selectKeepResolutionLowest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_resolution_lowest_clicked));
		m_refActionGroup->add_action("selectKeepResolutionHighest", sigc::mem_fun(*this, &SearchResultsPane::on_smart_select_resolution_highest_clicked));
		m_treeView.setMode(DuplicatesTreeView::Mode_t::Images);
	} else
	{
		m_refActionGroup->remove_action("selectKeepResolutionLowest");
		m_refActionGroup->remove_action("selectKeepResolutionHighest");
		m_treeView.setMode(DuplicatesTreeView::Mode_t::Normal);
	}
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_duplicate_selected(const Glib::ustring & filePath, const Glib::ustring & filePathLocked)
{
	if (filePath == filePathLocked)
	{
		m_previewPanel.remove(m_filePreviewRight);
		m_filePreviewLeft.setFile(filePath);
	} else
	{
		m_filePreviewLeft.setFile(filePathLocked);
		m_filePreviewRight.setFile(filePath);
		m_previewPanel.pack2(m_filePreviewRight, true, true);
		int handleWidth = 0;
		m_previewPanel.get_style_property("handle-size", handleWidth);
		m_previewPanel.set_position( std::lround((m_previewPanel.get_allocated_width() - handleWidth) / 2.0f) );
	}
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_expand_all_clicked()
{
	freeze_child_notify();
	freeze_notify();

	m_treeView.expand_all();

	thaw_notify();
	thaw_child_notify();
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_collapse_all_clicked()
{
	freeze_child_notify();
	freeze_notify();

	m_treeView.collapse_all();

	thaw_notify();
	thaw_child_notify();
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_delete_permanently()
{
	Gtk::MessageDialog messageDialog(_("Selected files will be permanently removed. Continue?"),
			false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
	if (m_treeView.atLeastOneTopLevelItemChecked())
	{
		messageDialog.set_secondary_text(_("Warning: you have selected all files in one or more duplicate groups. This means that all these files will be deleted."));
	}
	Gtk::Window * parent = dynamic_cast<Gtk::Window*>(get_toplevel());
	if (parent != nullptr) messageDialog.set_transient_for(*parent);

	if (messageDialog.run() == Gtk::RESPONSE_YES)
	{
		messageDialog.hide();
		process_deletion_result(execute_duplicate_action(_("Deleting…"), m_treeView.deletePermanently()));
	}
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_delete_move_to_trash()
{
	Gtk::MessageDialog messageDialog(_("Selected files will be moved to trash. Continue?"),
			false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
	if (m_treeView.atLeastOneTopLevelItemChecked())
	{
		messageDialog.set_secondary_text(_("Warning: you have selected all files in one or more duplicate groups. This means that all these files will be deleted."));
	}
	Gtk::Window * parent = dynamic_cast<Gtk::Window*>(get_toplevel());
	if (parent != nullptr) messageDialog.set_transient_for(*parent);

	if (messageDialog.run() == Gtk::RESPONSE_YES)
	{
		messageDialog.hide();
		process_deletion_result(execute_duplicate_action(_("Moving to trash…"), m_treeView.deleteToTrash()));
	}
}


//------------------------------------------------------------------------------
void
SearchResultsPane::on_delete_move_to_folder()
{
	Gtk::FileChooserDialog fileChooser(_("Select a backup folder"), Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	fileChooser.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	Gtk::Button * pButtonOk = fileChooser.add_button(_("OK"), Gtk::RESPONSE_OK);
	// mark "OK" button as a suggested action
	if (pButtonOk && pButtonOk->get_style_context()) pButtonOk->get_style_context()->add_class("suggested-action");
	Gtk::Window * pParent = dynamic_cast<Gtk::Window*>(get_toplevel());
	if (pParent != nullptr) fileChooser.set_transient_for(*pParent);
	if (fileChooser.run() == Gtk::RESPONSE_OK)
	{
		const auto backupFolder = fileChooser.get_filename();
		fileChooser.hide();
		process_deletion_result(execute_duplicate_action(_("Moving…"), m_treeView.deleteToBackupFolder(backupFolder, pParent)));
	}
}


//------------------------------------------------------------------------------
void
SearchResultsPane::process_deletion_result(callbacks::IDeferredAction::Result_t result)
{
	if (m_treeView.empty()) init();
	updateStatus();

	if ( (result == callbacks::IDeferredAction::Result_t::Failure) ||
	     (result == callbacks::IDeferredAction::Result_t::Mixed) )
	{
		Gtk::MessageDialog aDialog(_("Some of the files could not be deleted"), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
		aDialog.set_secondary_text(_("Please verify that sufficient permissions have been granted for your account."));
		Gtk::Window * pParent = dynamic_cast<Gtk::Window*>(get_toplevel());
		if (pParent != nullptr) aDialog.set_transient_for(*pParent);
		aDialog.run();
	}
}


//------------------------------------------------------------------------------
callbacks::IDuplicateReceiver::Ptr_t
SearchResultsPane::createPopulationDelegate()
{
	return m_treeView.createPopulationDelegate();
}


}}
