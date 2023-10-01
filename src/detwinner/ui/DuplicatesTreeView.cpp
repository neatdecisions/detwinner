/*
 ===============================================================================
 Name        : DuplicatesTreeView.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/DuplicatesTreeView.hpp>

#include <functional>
#include <set>

#include <glibmm/i18n.h>
#include <glibmm/miscutils.h>

#include <tools/BackupFileDeleter.hpp>
#include <tools/IconManager.hpp>
#include <tools/PermanentFileDeleter.hpp>
#include <tools/TrashFileDeleter.hpp>

namespace detwinner::ui {

//==============================================================================
// DuplicatesTreeView
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::DuplicatesTreeView()
		: m_store(Gtk::TreeStore::create(m_columns)), m_refBuilder(Gtk::Builder::create()),
			m_refActionGroup(Gio::SimpleActionGroup::create()), m_mode(Mode::Normal), m_adapted(false),
			m_columnResolution(nullptr)
{
	set_model(m_store);
	m_refBuilder->add_from_resource("/com/neatdecisions/detwinner/ui/duplicateTreeViewMenu.ui");
	m_refDuplicateFileMenu = loadMenu("menu-duplicate-file");
	m_refDuplicateGroupMenu = loadMenu("menu-duplicate-group");

	m_refActionGroup->add_action("file_open", sigc::mem_fun(*this, &DuplicatesTreeView::on_duplicate_file_open));
	m_refActionGroup->add_action("file_openfolder",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_duplicate_file_open_folder));
	m_refActionGroup->add_action("group_smartselect_first",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_first));
	m_refActionGroup->add_action("group_smartselect_last",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_last));
	m_refActionGroup->add_action("group_smartselect_earliest",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_earliest));
	m_refActionGroup->add_action("group_smartselect_latest",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_latest));
	m_refActionGroup->add_action("group_smartselect_shortestname",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_shortestname));
	m_refActionGroup->add_action("group_smartselect_longestname",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_longestname));
	m_refActionGroup->add_action("group_smartselect_shortestpath",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_shortestpath));
	m_refActionGroup->add_action("group_smartselect_longestpath",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_longestpath));

	m_refActionGroup->add_action("group_exclude", sigc::mem_fun(*this, &DuplicatesTreeView::on_group_exclude));
	m_refActionGroup->add_action("group_selectall", sigc::mem_fun(*this, &DuplicatesTreeView::on_group_select_all));
	m_refActionGroup->add_action("group_clearselection",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_clear_selection));
	m_refActionGroup->add_action("group_invertselection",
	                             sigc::mem_fun(*this, &DuplicatesTreeView::on_group_invert_selection));

	insert_action_group("duplicatestreeview", m_refActionGroup);

	m_fileNameRenderer.set_property("ellipsize", Pango::ELLIPSIZE_END);
	m_pathRenderer.set_property("ellipsize", Pango::ELLIPSIZE_END);
	m_sizeRenderer.set_property("ellipsize", Pango::ELLIPSIZE_END);
	m_resolutionRenderer.set_property("ellipsize", Pango::ELLIPSIZE_END);
	m_updateTimeRenderer.set_property("ellipsize", Pango::ELLIPSIZE_END);

	m_toggleRenderer.signal_toggled().connect(sigc::mem_fun(*this, &DuplicatesTreeView::on_cell_toggled));
	auto pColumn_Name = Gtk::make_managed<Gtk::TreeViewColumn>(_("Name"));
	pColumn_Name->pack_start(m_toggleRenderer, false);
	pColumn_Name->pack_end(m_fileNameRenderer);
	pColumn_Name->pack_end(m_iconRenderer, false);
	pColumn_Name->set_cell_data_func(m_fileNameRenderer, sigc::mem_fun(*this, &DuplicatesTreeView::on_render_filename));
	pColumn_Name->set_cell_data_func(m_iconRenderer, sigc::mem_fun(*this, &DuplicatesTreeView::on_render_icon));
	pColumn_Name->set_cell_data_func(m_toggleRenderer, sigc::mem_fun(*this, &DuplicatesTreeView::on_render_toggle));
	pColumn_Name->set_resizable(true);
	pColumn_Name->set_min_width(128);
	pColumn_Name->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	append_column(*pColumn_Name);

	m_lockRenderer.set_property("follow-state", true);
	auto pColumn_PreviewLock = Gtk::make_managed<Gtk::TreeViewColumn>("", m_lockRenderer);
	pColumn_PreviewLock->set_cell_data_func(m_lockRenderer, sigc::mem_fun(*this, &DuplicatesTreeView::on_render_lock));
	pColumn_PreviewLock->set_resizable(false);
	pColumn_PreviewLock->set_min_width(24);
	pColumn_PreviewLock->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	append_column(*pColumn_PreviewLock);

	auto pColumn_Path = Gtk::make_managed<Gtk::TreeViewColumn>(_("Path"), m_pathRenderer);
	pColumn_Path->set_cell_data_func(m_pathRenderer, sigc::mem_fun(*this, &DuplicatesTreeView::on_render_path));
	pColumn_Path->set_resizable(true);
	pColumn_Path->set_min_width(128);
	pColumn_Path->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	append_column(*pColumn_Path);

	auto pColumn_Size = Gtk::make_managed<Gtk::TreeViewColumn>(_("Size"), m_sizeRenderer);
	pColumn_Size->set_cell_data_func(m_sizeRenderer, sigc::mem_fun(*this, &DuplicatesTreeView::on_render_size));
	pColumn_Size->set_resizable(true);
	pColumn_Size->set_min_width(64);
	pColumn_Size->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	append_column(*pColumn_Size);

	m_columnResolution = Gtk::make_managed<Gtk::TreeViewColumn>(_("Resolution"), m_resolutionRenderer);
	m_columnResolution->set_cell_data_func(m_resolutionRenderer,
	                                       sigc::mem_fun(*this, &DuplicatesTreeView::on_render_resolution));
	m_columnResolution->set_resizable(true);
	m_columnResolution->set_min_width(64);
	m_columnResolution->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	append_column(*m_columnResolution);

	auto pColumn_UpdateTime = Gtk::make_managed<Gtk::TreeViewColumn>(_("Updated"), m_updateTimeRenderer);
	pColumn_UpdateTime->set_cell_data_func(m_updateTimeRenderer,
	                                       sigc::mem_fun(*this, &DuplicatesTreeView::on_render_update_time));
	pColumn_UpdateTime->set_resizable(true);
	pColumn_UpdateTime->set_min_width(64);
	pColumn_UpdateTime->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	append_column(*pColumn_UpdateTime);

	m_store->set_sort_column(GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID, Gtk::SORT_ASCENDING);
	set_activate_on_single_click(true);
	set_fixed_height_mode(true);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::~DuplicatesTreeView() noexcept
{
	if (m_refDuplicateFileMenu && m_refDuplicateFileMenu->get_attach_widget()) m_refDuplicateFileMenu->detach();
	if (m_refDuplicateGroupMenu && m_refDuplicateGroupMenu->get_attach_widget()) m_refDuplicateGroupMenu->detach();
}

//------------------------------------------------------------------------------
std::unique_ptr<Gtk::Menu>
DuplicatesTreeView::loadMenu(const Glib::ustring & name)
{
	Glib::RefPtr<Glib::Object> object = m_refBuilder->get_object(name);
	Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
	std::unique_ptr<Gtk::Menu> result = std::make_unique<Gtk::Menu>(gmenu);
	if (!result->get_attach_widget())
	{
		result->attach_to_widget(*this);
	}
	return result;
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::setMode(Mode mode)
{
	m_mode = mode;
	if (m_columnResolution != nullptr)
	{
		m_columnResolution->set_visible(m_mode == Mode::Images);
	}

	if (m_mode == Mode::Images)
	{
		m_refActionGroup->add_action(
				"group_smartselect_lowestresolution",
				sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_lowestresolution));
		m_refActionGroup->add_action(
				"group_smartselect_highestresolution",
				sigc::mem_fun(*this, &DuplicatesTreeView::on_group_smart_select_keep_highestresolution));
	} else
	{
		m_refActionGroup->remove_action("group_smartselect_lowestresolution");
		m_refActionGroup->remove_action("group_smartselect_highestresolution");
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_hide()
{
	m_adapted = false;
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_size_allocate(Gtk::Allocation & s)
{
	Gtk::TreeView::on_size_allocate(s);
	if (!m_adapted)
	{
		const int width = s.get_width() - 2 * get_border_width() - get_column(1)->get_width();
		if (width > 0)
		{
			get_column(0)->set_fixed_width(std::floor(0.35f * width));
			get_column(2)->set_fixed_width(std::floor(0.35f * width));
			get_column(3)->set_fixed_width(std::floor(0.1f * width));
		}
		m_adapted = true;
	}
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::on_button_press_event(GdkEventButton * button_event)
{
	constexpr unsigned int kRightButton = 3;

	bool result = Gtk::TreeView::on_button_press_event(button_event);
	if ((button_event != nullptr) && (button_event->type == GDK_BUTTON_PRESS) && (button_event->button == kRightButton))
	{
		Gtk::TreeIter iter = getSelectedIter();
		if (isDuplicateFileIter(iter))
		{
			if (m_refDuplicateFileMenu) m_refDuplicateFileMenu->popup_at_pointer(reinterpret_cast<GdkEvent *>(button_event));
			result = true;
		} else if (iter && !iter->parent() && !iter->children().empty())
		{
			if (m_refDuplicateFileMenu) m_refDuplicateGroupMenu->popup_at_pointer(reinterpret_cast<GdkEvent *>(button_event));
		}
	}
	return result;
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_toggle(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr) return;
	const CheckState checkState = iter ? (*iter)[m_columns.checkState] : CheckState::Unchecked;
	switch (checkState)
	{
	case CheckState::Checked:
		cellRenderer->set_property("active", true);
		cellRenderer->set_property("inconsistent", false);
		break;
	case CheckState::Unchecked:
		cellRenderer->set_property("active", false);
		cellRenderer->set_property("inconsistent", false);
		break;
	default:
		cellRenderer->set_property("active", false);
		cellRenderer->set_property("inconsistent", true);
		break;
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_size(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr || !iter) return;
	cellRenderer->set_property("text", Glib::format_size((*iter)[m_columns.fileSize]));
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_resolution(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr || !iter) return;
	if (iter->parent() && !iter->children() && ((*iter)[m_columns.width] > 0) && ((*iter)[m_columns.height] > 0))
	{
		cellRenderer->set_property("text",
		                           Glib::ustring::compose("%1×%2", (*iter)[m_columns.width], (*iter)[m_columns.height]));
	} else
	{
		cellRenderer->set_property("text", Glib::ustring(""));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_icon(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr) return;
	cellRenderer->set_property("pixbuf", Glib::RefPtr<Gdk::Pixbuf>());
	cellRenderer->set_property("icon-name", Glib::ustring("text-x-generic"));
	if (!iter) return;

	auto parent = iter->parent();
	if (parent)
	{
		const Glib::RefPtr<Gdk::Pixbuf> icon =
				tools::IconManager::GetInstance().getFileIcon(Glib::ustring((*iter)[m_columns.filePath]), 16);
		if (icon) cellRenderer->set_property("pixbuf", icon);
	} else
	{
		cellRenderer->set_property("icon-name", Glib::ustring("folder-saved-search"));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_lock(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr || !iter) return;
	auto parent = iter->parent();
	if (parent)
	{
		if ((*parent)[m_columns.locked])
		{
			if ((*iter)[m_columns.locked])
			{
				cellRenderer->set_property("icon-name", Glib::ustring("changes-prevent"));
				cellRenderer->set_sensitive(true);
			} else
			{
				cellRenderer->set_property("icon-name", Glib::ustring(""));
			}
		} else
		{
			cellRenderer->set_property("icon-name", Glib::ustring("changes-allow"));
			cellRenderer->set_sensitive(false);
		}
	} else
	{
		cellRenderer->set_property("icon-name", Glib::ustring(""));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_filename(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr || !iter) return;
	CheckState checkState = getCheck(iter);
	cellRenderer->set_property("strikethrough", checkState == CheckState::Checked);
	const auto & kids = iter->children();
	if (kids && !kids.empty())
	{
		const auto mm = std::minmax_element(kids.begin(), kids.end(), [this](const auto & v1, const auto & v2) {
			return v1[m_columns.fileSize] < v2[m_columns.fileSize];
		});
		const unsigned long long minSize = (*mm.first)[m_columns.fileSize];
		const unsigned long long maxSize = (*mm.second)[m_columns.fileSize];

		if (maxSize == minSize)
		{
			cellRenderer->set_property(
					"text", Glib::ustring::compose(_("%1 files, %2 each"), kids.size(), Glib::format_size(maxSize)));
		} else
		{
			cellRenderer->set_property("text",
			                           Glib::ustring::compose(_("%1 files, from %2 to %3"), kids.size(),
			                                                  Glib::format_size(minSize), Glib::format_size(maxSize)));
		}

	} else
	{
		cellRenderer->set_property("text", Glib::path_get_basename(Glib::ustring((*iter)[m_columns.filePath])));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_path(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr || !iter) return;
	if (iter->children() && !iter->children().empty())
	{
		cellRenderer->set_property("text", Glib::ustring(""));
	} else
	{
		cellRenderer->set_property("text", Glib::path_get_dirname(Glib::ustring((*iter)[m_columns.filePath])));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_render_update_time(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr || !iter) return;
	if (iter->children() && !iter->children().empty())
	{
		cellRenderer->set_property("text", Glib::ustring(""));
	} else
	{
		cellRenderer->set_property("text", getUpdateTime(iter).format("%x %X"));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_cell_toggled(const Glib::ustring & path)
{
	auto iter = m_store->get_iter(path);
	if (!iter) return;
	auto item = *iter;
	CheckState newCheckState =
			(item[m_columns.checkState] == CheckState::Unchecked) ? CheckState::Checked : CheckState::Unchecked;
	setCheck(item, newCheckState, false, false);
}

//------------------------------------------------------------------------------
callbacks::IDuplicateReceiver::Ptr
DuplicatesTreeView::createPopulationDelegate()
{
	return std::make_shared<PopulationDelegate>(*this);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::CheckState
DuplicatesTreeView::getCheck(const Gtk::TreeIter & iter) const
{
	if (!iter) return CheckState::Unchecked;
	return (*iter)[m_columns.checkState];
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::setCheck(const Gtk::TreeIter & iter, CheckState checkState, bool noUp, bool noDown)
{
	if (!iter) return false;
	if (getCheck(iter) == checkState) return true;

	(*iter)[m_columns.checkState] = checkState;

	if (!noDown && (checkState != CheckState::Mixed))
	{
		for (Gtk::TreeIter kid : iter->children())
		{
			setCheck(kid, checkState, true, noDown);
		}
	}

	if (!noUp)
	{
		auto && parent = iter->parent();
		if (parent)
		{
			const auto & kids = parent->children();
			const bool all =
					std::all_of(kids.begin(), kids.end(), [this, checkState](auto it) { return getCheck(it) == checkState; });
			setCheck(parent, all ? checkState : CheckState::Mixed, noUp, true);
		}
	}

	return true;
}

//------------------------------------------------------------------------------
Glib::ustring
DuplicatesTreeView::extractFullPath(const Gtk::TreeIter & iter) const
{
	return (*iter)[m_columns.filePath];
}

//------------------------------------------------------------------------------
Glib::DateTime
DuplicatesTreeView::getUpdateTime(const Gtk::TreeIter & iter)
{
	auto row = *iter;
	if (!static_cast<Glib::DateTime>(row[m_columns.updateTime]))
	{
		const Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(extractFullPath(iter));
		const Glib::RefPtr<Gio::FileInfo> fileInfo = file->query_info("time::modified");
		if (fileInfo->has_attribute(G_FILE_ATTRIBUTE_TIME_MODIFIED))
		{
			row[m_columns.updateTime] =
					Glib::DateTime::create_now_local(fileInfo->get_attribute_uint64(G_FILE_ATTRIBUTE_TIME_MODIFIED));
		}
	}
	return row[m_columns.updateTime];
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::isDuplicateFileIter(const Gtk::TreeIter & iter) const
{
	return iter && iter->parent() && iter->children().empty();
}

//------------------------------------------------------------------------------
Gtk::TreeIter
DuplicatesTreeView::getSelectedIter() const
{
	auto pTreeSelection = get_selection();
	if (pTreeSelection)
	{
		const auto & selectedRows = pTreeSelection->get_selected_rows();
		if (selectedRows.size() == 1)
		{
			return m_store->get_iter(selectedRows.front());
		}
	}
	return Gtk::TreeIter();
}

//------------------------------------------------------------------------------
Glib::ustring
DuplicatesTreeView::getPreviewPath(const Gtk::TreeIter & iter) const
{
	Glib::ustring result;
	if (!iter) return result;
	const auto & duplicates = iter->children();
	if (!duplicates) return extractFullPath(iter);
	if ((*iter)[m_columns.locked])
	{
		auto it = std::find_if(duplicates.begin(), duplicates.end(), [this](auto val) { return val[m_columns.locked]; });
		if (it != duplicates.end())
		{
			result = extractFullPath(it);
		}
	} else
	{
		if (!duplicates.empty())
		{
			result = extractFullPath(duplicates.begin());
		}
	}
	return result;
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_row_activated(const Gtk::TreeModel::Path & path, Gtk::TreeViewColumn * treeColumn)
{
	Gtk::TreeView::on_row_activated(path, treeColumn);
	if (path.empty()) return;

	if (treeColumn == this->get_column(1))
	{
		auto iter = m_store->get_iter(path);
		auto parent = iter->parent();
		if (parent)
		{
			if ((*parent)[m_columns.locked])
			{
				if ((*iter)[m_columns.locked])
				{
					(*iter)[m_columns.locked] = false;
					(*parent)[m_columns.locked] = false;
				}
			} else
			{
				(*iter)[m_columns.locked] = true;
				(*parent)[m_columns.locked] = true;
			}
		}
		queue_draw();
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_cursor_changed()
{
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn * column = nullptr;
	get_cursor(path, column);
	if (path.empty()) return;

	auto iter = m_store->get_iter(path);
	if (!iter) return;

	const Glib::ustring pathToEmit = getPreviewPath(iter);

	Glib::ustring pathToEmitLocked;
	if (iter->parent() && (*iter->parent())[m_columns.locked])
	{
		pathToEmitLocked = getPreviewPath(iter->parent());
	}
	if (pathToEmitLocked.empty()) pathToEmitLocked = pathToEmit;

	m_signalDuplicateSelected.emit(pathToEmit, pathToEmitLocked);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::signal_duplicate_selected
DuplicatesTreeView::on_duplicate_selected()
{
	return m_signalDuplicateSelected;
}

//------------------------------------------------------------------------------
DuplicatesTreeView::signal_stats_changed
DuplicatesTreeView::on_stats_changed()
{
	return m_signalStatsChanged;
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_duplicate_file_open() const
{
	Gtk::TreeIter it = getSelectedIter();
	if (isDuplicateFileIter(it))
	{
		Gio::AppInfo::launch_default_for_uri(Glib::filename_to_uri(extractFullPath(it)));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_duplicate_file_open_folder() const
{
	Gtk::TreeIter it = getSelectedIter();
	if (isDuplicateFileIter(it))
	{
		Gio::AppInfo::launch_default_for_uri(Glib::filename_to_uri(Glib::path_get_dirname(extractFullPath(it))));
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_first()
{
	createSmartSelector_KeepFirstInGroup()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_last()
{
	createSmartSelector_KeepLastInGroup()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_earliest()
{
	createSmartSelector_KeepEarliestModified()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_latest()
{
	createSmartSelector_KeepLatestModified()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_shortestname()
{
	createSmartSelector_KeepShortestName()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_longestname()
{
	createSmartSelector_KeepLongestName()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_shortestpath()
{
	createSmartSelector_KeepShortestPath()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_longestpath()
{
	createSmartSelector_KeepLongestName()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_lowestresolution()
{
	createSmartSelector_KeepLowestResolution()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_smart_select_keep_highestresolution()
{
	createSmartSelector_KeepHighestResolution()->select(*this, getSelectedIter());
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_select_all()
{
	setCheck(getSelectedIter(), CheckState::Checked, false, false);
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_clear_selection()
{
	setCheck(getSelectedIter(), CheckState::Unchecked, false, false);
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_invert_selection()
{
	Gtk::TreeIter iter = getSelectedIter();
	if (!iter) return;
	const CheckState checkState = getCheck(iter);
	switch (checkState)
	{
	case CheckState::Checked:
		setCheck(iter, CheckState::Unchecked, false, false);
		break;
	case CheckState::Unchecked:
		setCheck(iter, CheckState::Checked, false, false);
		break;
	case CheckState::Mixed:
		for (auto && kid : iter->children())
		{
			setCheck(kid, getCheck(kid) == CheckState::Checked ? CheckState::Unchecked : CheckState::Checked, true, false);
		}
		break;
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::on_group_exclude()
{
	m_store->erase(getSelectedIter());
	m_signalStatsChanged.emit();
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::sortByTotalSize(bool asc)
{
	m_store->set_default_sort_func(sigc::mem_fun(*this, &DuplicatesTreeView::sort_func_total_size));
	m_store->set_sort_column(GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, asc ? Gtk::SORT_ASCENDING : Gtk::SORT_DESCENDING);
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::sortBySingleFileSize(bool asc)
{
	m_store->set_default_sort_func(sigc::mem_fun(*this, &DuplicatesTreeView::sort_func_single_file_size));
	m_store->set_sort_column(GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, asc ? Gtk::SORT_ASCENDING : Gtk::SORT_DESCENDING);
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::sortByNumberOfFiles(bool asc)
{
	m_store->set_default_sort_func(sigc::mem_fun(*this, &DuplicatesTreeView::sort_func_number_of_files));
	m_store->set_sort_column(GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, asc ? Gtk::SORT_ASCENDING : Gtk::SORT_DESCENDING);
}

//------------------------------------------------------------------------------
int
DuplicatesTreeView::sort_func_total_size(const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) const
{
	if (l == r) return 0;
	if (!l) return -1;
	if (!r) return 1;
	if (l->parent() && (l->parent() == r->parent())) return 0;
	const auto leftSize = (*l)[m_columns.fileSize];
	const auto rightSize = (*r)[m_columns.fileSize];
	if (leftSize < rightSize) return -1;
	if (leftSize > rightSize) return 1;

	return 0;
}

//------------------------------------------------------------------------------
int
DuplicatesTreeView::sort_func_single_file_size(const Gtk::TreeModel::iterator & l,
                                               const Gtk::TreeModel::iterator & r) const
{
	if (l == r) return 0;
	if (!l) return -1;
	if (!r) return 1;
	if (l->parent() && (l->parent() == r->parent())) return 0;
	const unsigned long long leftSize = (l->children().empty()) ? 0 : (*l)[m_columns.fileSize] / l->children().size();
	const unsigned long long rightSize = (r->children().empty()) ? 0 : (*r)[m_columns.fileSize] / r->children().size();

	if (leftSize < rightSize) return -1;
	if (leftSize > rightSize) return 1;

	return 0;
}

//------------------------------------------------------------------------------
int
DuplicatesTreeView::sort_func_number_of_files(const Gtk::TreeModel::iterator & l,
                                              const Gtk::TreeModel::iterator & r) const
{
	if (l == r) return 0;
	if (!l) return -1;
	if (!r) return 1;
	if (l->parent() && (l->parent() == r->parent())) return 0;
	if (l->children().size() < r->children().size()) return -1;
	if (l->children().size() > r->children().size()) return 1;

	return 0;
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepFirstInGroup()
{
	constexpr auto lambda = [](const Gtk::TreeModel::iterator &, const Gtk::TreeModel::iterator &) { return false; };
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepLastInGroup()
{
	constexpr auto lambda = [](const Gtk::TreeModel::iterator &, const Gtk::TreeModel::iterator &) { return true; };
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepEarliestModified()
{
	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return getUpdateTime(r).compare(getUpdateTime(l)) > 0;
	};
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepLatestModified()
{
	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return getUpdateTime(r).compare(getUpdateTime(l)) < 0;
	};
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepShortestName()
{
	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return Glib::path_get_basename(l->get_value(m_columns.filePath)).size() <
		       Glib::path_get_basename(r->get_value(m_columns.filePath)).size();
	};
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepLongestName()
{
	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return Glib::path_get_basename(l->get_value(m_columns.filePath)).size() >
		       Glib::path_get_basename(r->get_value(m_columns.filePath)).size();
	};
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepShortestPath()
{
	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return Glib::path_get_dirname(l->get_value(m_columns.filePath)).size() <
		       Glib::path_get_dirname(r->get_value(m_columns.filePath)).size();
	};
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepLongestPath()
{
	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return Glib::path_get_dirname(l->get_value(m_columns.filePath)).size() >
		       Glib::path_get_dirname(r->get_value(m_columns.filePath)).size();
	};
	return std::make_shared<SmartSelector>(lambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepLowestResolution()
{
	auto ignoreLambda = [this](const Gtk::TreeModel::iterator & l) {
		return (l->get_value(m_columns.width) == 0) || (l->get_value(m_columns.height) == 0);
	};

	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return (l->get_value(m_columns.width) * l->get_value(m_columns.height) <
		        r->get_value(m_columns.width) * r->get_value(m_columns.height));
	};
	return std::make_shared<SmartSelector>(lambda, ignoreLambda);
}

//------------------------------------------------------------------------------
DuplicatesTreeView::ISmartSelector::Ptr
DuplicatesTreeView::createSmartSelector_KeepHighestResolution()
{
	auto ignoreLambda = [this](const Gtk::TreeModel::iterator & l) {
		return (l->get_value(m_columns.width) == 0) || (l->get_value(m_columns.height) == 0);
	};

	auto lambda = [this](const Gtk::TreeModel::iterator & l, const Gtk::TreeModel::iterator & r) {
		return (l->get_value(m_columns.width) * l->get_value(m_columns.height) >
		        r->get_value(m_columns.width) * r->get_value(m_columns.height));
	};
	return std::make_shared<SmartSelector>(lambda, ignoreLambda);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepFirstInGroup()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepFirstInGroup(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepLastInGroup()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepLastInGroup(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepEarliestModified()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepEarliestModified(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepLatestModified()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepLatestModified(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepShortestName()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepShortestName(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepLongestName()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepLongestName(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepShortestPath()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepShortestPath(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepLongestPath()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepLongestPath(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepLowestResolution()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepLowestResolution(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::smartSelect_KeepHighestResolution()
{
	return std::make_shared<TreeSelect_Smart>(createSmartSelector_KeepHighestResolution(), *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::selectAll()
{
	constexpr auto lambda = [](const Gtk::TreeModel::iterator &) { return CheckState::Checked; };
	return std::make_shared<TreeSelect_Bulk>(lambda, *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::clearSelection()
{
	constexpr auto lambda = [](const Gtk::TreeModel::iterator &) { return CheckState::Unchecked; };
	return std::make_shared<TreeSelect_Bulk>(lambda, *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::invertSelection()
{
	auto lambda = [this](const Gtk::TreeModel::iterator & iter) {
		return (getCheck(iter) == CheckState::Unchecked) ? CheckState::Checked : CheckState::Unchecked;
	};
	return std::make_shared<TreeSelect_Bulk>(lambda, *this);
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::deletePermanently()
{
	return std::make_shared<TreeDeleteAction>(*this, std::make_shared<tools::PermanentFileDeleter>());
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::deleteToTrash()
{
	return std::make_shared<TreeDeleteAction>(*this, std::make_shared<tools::TrashFileDeleter>());
}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::deleteToBackupFolder(const std::string & folder, Gtk::Window * dialogParent)
{
	return std::make_shared<TreeDeleteAction>(*this, std::make_shared<tools::BackupFileDeleter>(folder, dialogParent));
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::clear()
{
	unset_model();
	m_store->set_sort_column(GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID, Gtk::SORT_ASCENDING);
	m_store->unset_default_sort_func();
	m_store->clear();
	set_model(m_store);
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::empty() const
{
	return m_store->children().empty();
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::atLeastOneTopLevelItemChecked() const
{
	const auto & kids = m_store->children();
	return std::any_of(kids.begin(), kids.end(), [this](auto it) { return getCheck(it) == CheckState::Checked; });
}

//------------------------------------------------------------------------------
DuplicatesTreeView::DuplicateStats
DuplicatesTreeView::calculateStats() const
{
	DuplicateStats result;
	result.groupCount = m_store->children().size();
	if (m_mode == Mode::Normal)
	{
		for (const auto & group : m_store->children())
		{
			result.fileCount += group.children().size();
			result.totalSize += group[m_columns.fileSize];
			if (group.children().size() > 1)
			{
				result.wastedSize += (group[m_columns.fileSize] - group.children().begin()->get_value(m_columns.fileSize));
			}
		}
	} else if (m_mode == Mode::Images)
	{
		for (const auto & group : m_store->children())
		{
			const auto & items = group.children();
			result.fileCount += items.size();
			const unsigned long long totalGroupSize = group[m_columns.fileSize];
			result.totalSize += totalGroupSize;
			if (items.size() > 1)
			{
				auto maxElement =
						std::max_element(items.begin(), items.end(), [this](const Gtk::TreeIter & l, const Gtk::TreeIter & r) {
							return (*l)[this->m_columns.fileSize] < (*r)[this->m_columns.fileSize];
						});
				if (maxElement != items.end())
				{
					const unsigned long long maxGroupFileSize = (*maxElement)[m_columns.fileSize];
					if (totalGroupSize > maxGroupFileSize)
					{
						result.wastedSize += (totalGroupSize - maxGroupFileSize);
					}
				}
			}
		}
	}
	return result;
}

//==============================================================================
// DuplicatesTreeView::SmartSelector
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::SmartSelector::SmartSelector(const SelectFunc & selectFunc, const IgnoreFunc & ignoreFunc)
		: m_selectFunc(selectFunc), m_ignoreFunc(ignoreFunc)
{
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::SmartSelector::select(DuplicatesTreeView & tree, Gtk::TreeIter iter)
{
	if (!iter) return;
	auto keepIter = iter->children().end();
	std::vector<Gtk::TreeModel::iterator> ignoreIterators;
	for (auto file : iter->children())
	{
		if (m_ignoreFunc(file))
		{
			ignoreIterators.push_back(file);
			continue;
		}
		if (keepIter)
		{
			if (m_selectFunc(file, keepIter))
			{
				keepIter = file;
			}
		} else
		{
			keepIter = file;
		}
	}

	if (keepIter)
	{
		tree.setCheck(iter, CheckState::Checked, true, false);
		tree.setCheck(keepIter, CheckState::Unchecked, false, true);
		for (auto && ignored : ignoreIterators)
		{
			tree.setCheck(ignored, CheckState::Unchecked, false, true);
		}
	} else
	{
		tree.setCheck(iter, CheckState::Unchecked, true, false);
	}
}

//==============================================================================
// DuplicatesTreeView::PopulationDelegate
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::PopulationDelegate::PopulationDelegate(DuplicatesTreeView & tree) : m_tree(tree) {}

//------------------------------------------------------------------------------
callbacks::IDeferredAction::Ptr
DuplicatesTreeView::PopulationDelegate::populate(logic::DuplicatesList && container)
{
	m_tree.sortByTotalSize(false);
	return std::make_shared<TreePopulateAction>(m_tree, std::move(container));
}

//==============================================================================
// DuplicatesTreeView::TreeAction
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::TreeAction::TreeAction(DuplicatesTreeView & tree)
		: m_tree(tree), m_totalItems(m_tree.m_store->children().size()), m_currentItem(0),
			m_iter(tree.m_store->children().begin()), m_resultStatus(Result::Success)
{
}

//------------------------------------------------------------------------------
double
DuplicatesTreeView::TreeAction::getProgress() const
{
	if (m_totalItems == 0) return 1.0;
	return static_cast<double>(m_currentItem) / m_totalItems;
}

//------------------------------------------------------------------------------
DuplicatesTreeView::TreeAction::Result
DuplicatesTreeView::TreeAction::getStatus() const
{
	return m_resultStatus;
}

//==============================================================================
// DuplicatesTreeView::TreeSelect_Smart
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::TreeSelect_Smart::TreeSelect_Smart(const ISmartSelector::Ptr & smartSelector,
                                                       DuplicatesTreeView & tree)
		: TreeAction(tree), m_smartSelector(smartSelector)
{
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::TreeSelect_Smart::processNext()
{
	if (!m_iter || !m_smartSelector) return false;
	m_smartSelector->select(m_tree, m_iter);
	++m_currentItem;
	return static_cast<bool>(++m_iter);
}

//==============================================================================
// DuplicatesTreeView::TreeSelect_Bulk
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::TreeSelect_Bulk::TreeSelect_Bulk(const SelectFunc & selectFunc, DuplicatesTreeView & tree)
		: TreeAction(tree), m_selectFunc(selectFunc)
{
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::TreeSelect_Bulk::processNext()
{
	if (!m_iter) return false;
	std::set<CheckState> states;
	for (auto file : m_iter->children())
	{
		CheckState newCheck = m_selectFunc(file);
		states.insert(newCheck);
		m_tree.setCheck(file, newCheck, true, true);
	}
	m_tree.setCheck(m_iter, (states.size() == 1) ? *states.begin() : CheckState::Mixed, true, true);

	++m_currentItem;
	return static_cast<bool>(++m_iter);
}

//==============================================================================
// DuplicatesTreeView::TreePopulateAction
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::TreePopulateAction::TreePopulateAction(DuplicatesTreeView & tree, logic::DuplicatesList && values)
		: m_tree(tree), m_values(std::move(values)), m_currentItem(0), m_batchStarted(false)
{
}

//------------------------------------------------------------------------------
DuplicatesTreeView::TreePopulateAction::~TreePopulateAction() noexcept { endBatch(); }

//------------------------------------------------------------------------------
double
DuplicatesTreeView::TreePopulateAction::getProgress() const
{
	const std::size_t totalSize = m_values.size();
	if (totalSize > 0) return static_cast<double>(m_currentItem) / totalSize;
	return 1.0;
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::TreePopulateAction::processNext()
{
	if (m_currentItem >= m_values.size())
	{
		endBatch();
		return false;
	}

	beginBatch();

	constexpr std::chrono::milliseconds kMaxDuration(500);
	auto currentTime = std::chrono::steady_clock::now();
	while ((m_currentItem < m_values.size()) && ((std::chrono::steady_clock::now() - currentTime) < kMaxDuration))
	{
		const logic::DuplicateContainer & duplicateGroup = m_values[m_currentItem];
		Gtk::TreeRow treeRow = *m_tree.m_store->append();

		treeRow[m_tree.m_columns.checkState] = CheckState::Unchecked;
		treeRow[m_tree.m_columns.locked] = true;

		unsigned long long totalSize = 0;
		bool locked = true;
		for (const auto & duplicateFile : duplicateGroup.files)
		{
			auto t = *m_tree.m_store->append(treeRow->children());
			t[m_tree.m_columns.checkState] = CheckState::Unchecked;
			t[m_tree.m_columns.filePath] = duplicateFile.name;
			t[m_tree.m_columns.fileSize] = duplicateFile.size;
			if (duplicateFile.imageResolution)
			{
				t[m_tree.m_columns.width] = duplicateFile.imageResolution->width;
				t[m_tree.m_columns.height] = duplicateFile.imageResolution->height;
			}
			t[m_tree.m_columns.locked] = locked;
			locked = false;

			totalSize += duplicateFile.size;
		}
		treeRow[m_tree.m_columns.fileSize] = totalSize;

		++m_currentItem;
	}
	return true;
}

//------------------------------------------------------------------------------
DuplicatesTreeView::TreePopulateAction::Result
DuplicatesTreeView::TreePopulateAction::getStatus() const
{
	return Result::Success;
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::TreePopulateAction::beginBatch()
{
	if (!m_batchStarted)
	{
		m_batchStarted = true;
		m_tree.freeze_notify();
		m_tree.freeze_child_notify();
		m_tree.unset_model();
	}
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::TreePopulateAction::endBatch()
{
	if (m_batchStarted)
	{
		m_batchStarted = false;
		m_tree.set_model(m_tree.m_store);
		m_tree.thaw_child_notify();
		m_tree.thaw_notify();
	}
}

//==============================================================================
// DuplicatesTreeView::TreeDeleteAction
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesTreeView::TreeDeleteAction::TreeDeleteAction(DuplicatesTreeView & tree,
                                                       tools::AbstractFileDeleter::Ptr fileDeleter)
		: DuplicatesTreeView::TreeAction(tree), m_fileDeleter(fileDeleter)
{
	m_resultStatus = Result::Unknown;
}

//------------------------------------------------------------------------------
bool
DuplicatesTreeView::TreeDeleteAction::processNext()
{
	if (!m_iter) return false;
	if (!m_fileDeleter) return false;
	if (m_tree.getCheck(m_iter) != CheckState::Unchecked)
	{
		auto it = m_iter->children().begin();
		while (it)
		{
			if (m_tree.getCheck(it) == CheckState::Checked)
			{
				if (m_fileDeleter->removeFile(it->get_value(m_tree.m_columns.filePath)))
				{
					it = m_tree.m_store->erase(it);
					updateResultStatus(Result::Success);
					continue;
				} else
				{
					updateResultStatus(Result::Failure);
				}
			}
			++it;
		}
	}

	// group with one or no files, remove it as well
	if (m_iter->children().size() <= 1)
	{
		m_iter = m_tree.m_store->erase(m_iter);
		return static_cast<bool>(m_iter);
	}

	// update check states of a group
	std::set<CheckState> states;
	for (auto file : m_iter->children())
	{
		states.insert(m_tree.getCheck(file));
	}
	if (states.size() == 1)
	{
		m_tree.setCheck(m_iter, *states.begin(), true, true);
	} else
	{
		m_tree.setCheck(m_iter, CheckState::Mixed, true, true);
	}

	return static_cast<bool>(++m_iter);
}

//------------------------------------------------------------------------------
void
DuplicatesTreeView::TreeDeleteAction::updateResultStatus(Result stepStatus)
{
	if (stepStatus == Result::Mixed)
	{
		m_resultStatus = Result::Mixed;
	} else
	{
		switch (m_resultStatus)
		{
		case Result::Unknown:
			m_resultStatus = stepStatus;
			break;
		case Result::Failure:
			if (stepStatus == Result::Success) m_resultStatus = Result::Mixed;
			break;
		case Result::Success:
			if (stepStatus == Result::Failure) m_resultStatus = Result::Mixed;
			break;
		case Result::Mixed:
			break;
		}
	}
}

} // namespace detwinner::ui
