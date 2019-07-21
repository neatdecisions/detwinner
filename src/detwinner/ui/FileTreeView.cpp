/*
 ===============================================================================
 Name        : FileTreeView.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/FileTreeView.hpp>

#include <giomm/file.h>
#include <glibmm/miscutils.h>
#include <tools/IconManager.hpp>


namespace detwinner {
namespace ui {


//------------------------------------------------------------------------------
FileTreeView::FileTreeView() :
	m_store(Gtk::TreeStore::create(m_columns)),
	m_showHiddenFiles(false)
{
	set_model(m_store);

	Gtk::TreeViewColumn * pColumn = Gtk::manage(new Gtk::TreeViewColumn(""));
	Gtk::CellRendererToggle * pRendererToggle = Gtk::manage(new Gtk::CellRendererToggle());
	Gtk::CellRendererText * pRendererText = Gtk::manage(new Gtk::CellRendererText());
	Gtk::CellRendererPixbuf * pRendererPixbuf = Gtk::manage(new Gtk::CellRendererPixbuf());
	pRendererToggle->signal_toggled().connect( sigc::mem_fun(*this, &FileTreeView::on_cell_toggled) );
	pColumn->pack_start(*pRendererToggle, false);
	pColumn->pack_end(*pRendererText, false);
	pColumn->pack_end(*pRendererPixbuf, false);
	pColumn->set_cell_data_func(*pRendererToggle, sigc::mem_fun(*this, &FileTreeView::on_render_toggle));
	pColumn->set_cell_data_func(*pRendererText, sigc::mem_fun(*this, &FileTreeView::on_render_filename));
	pColumn->set_cell_data_func(*pRendererPixbuf, sigc::mem_fun(*this, &FileTreeView::on_render_icon));
	pColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	append_column(*pColumn);

	set_headers_visible(false);
	set_fixed_height_mode(true);
}


//------------------------------------------------------------------------------
void
FileTreeView::on_render_filename(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr) return;
	cellRenderer->set_property("text", getFileName(iter));
}


//------------------------------------------------------------------------------
void
FileTreeView::on_render_toggle(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr) return;
	const CheckState_t checkState = iter ? (*iter)[m_columns.checkState] : CheckState_t::Unchecked;
	switch (checkState)
	{
	case CheckState_t::Checked:
		cellRenderer->set_property("active", true);
		cellRenderer->set_property("inconsistent", false);
		break;
	case CheckState_t::Unchecked:
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
FileTreeView::on_render_icon(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter)
{
	if (cellRenderer == nullptr) return;
	cellRenderer->set_property("pixbuf", Glib::RefPtr<Gdk::Pixbuf>());
	cellRenderer->set_property("icon-name", Glib::ustring("text-x-generic"));
	if (!iter) return;

	const Glib::RefPtr<Gdk::Pixbuf> iconPixBuf = tools::IconManager::GetInstance().getFileIcon(Glib::ustring((*iter)[m_columns.fullPath]), 16);
	if (iconPixBuf) cellRenderer->set_property("pixbuf", iconPixBuf);
}


//------------------------------------------------------------------------------
std::string
FileTreeView::getFileName(const Gtk::TreeModel::iterator & iter)
{
	return iter ?
			Glib::path_get_basename(Glib::ustring((*iter)[m_columns.fullPath])) :
			std::string();
}


//------------------------------------------------------------------------------
std::vector<std::string>
FileTreeView::fetchSelectedPaths() const
{
	std::vector<std::string> paths;
	for (auto && row : m_store->children())
	{
		fetchSelectedPathsInternal(row, paths);
	}
	return paths;
}


//------------------------------------------------------------------------------
void
FileTreeView::fetchSelectedPathsInternal(const Gtk::TreeRow & treeRow, std::vector<std::string> & paths) const
{
	if (getCheck(treeRow) == CheckState_t::Mixed)
	{
		for (auto && row : treeRow.children())
		{
			fetchSelectedPathsInternal(row, paths);
	}
	} else
	{
		if ( (getCheck(treeRow) == CheckState_t::Checked) && !treeRow[m_columns.fake] )
		{
			paths.push_back(static_cast<Glib::ustring>(treeRow[m_columns.fullPath]));
		}
	}
}


//------------------------------------------------------------------------------
void
FileTreeView::on_row_expanded(const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	Gtk::TreeView::on_row_expanded(iter, path);
}


//------------------------------------------------------------------------------
void
FileTreeView::refresh_children(const Gtk::TreeModel::iterator & iter)
{
	if ( iter->children().empty() ) return;
	if ( (iter->children().size() == 1) && (*iter->children().begin())[m_columns.fake] ) return;
	reload_from_filesystem(iter);
	for (auto && kid: iter->children())
	{
		refresh_children(kid);
	}
}


//------------------------------------------------------------------------------
void
FileTreeView::reload_from_filesystem(const Gtk::TreeModel::iterator & iter)
{
	if (!iter) return;
	auto row = *iter;

	const std::string & filePath = Glib::ustring(row[m_columns.fullPath]);

	for (auto it = row.children().begin(); it; )
	{
		Gtk::TreeRow childRow = *it;
		if (childRow[m_columns.fake])
		{
			it = m_store->erase(it);
		} else
		{
			++it;
		}
	}

	const CheckState_t checkState = getCheck(iter);

	fillChildren(filePath, checkState, row);

	// take into account the case when node had a single subnode checked
	// and this subnode has been deleted, so node checkstate should change
	// from half-checked to unchecked
	// and the similar case when a single unchecked subnode is deleted
	if (checkState == CheckState_t::Mixed)
	{
		bool allUnchecked = true;
		bool allChecked = true;
		for (auto kid : row.children())
		{
			if (getCheck(kid) != CheckState_t::Unchecked)
			{
				allUnchecked = false;
			}
			if (getCheck(kid) != CheckState_t::Checked)
			{
				allChecked = false;
			}
			if (!allChecked && !allUnchecked)
				break;
		}
		if (allUnchecked)
		{
			setCheck(row, CheckState_t::Unchecked, false, true);
		} else
		{
			if (allChecked)
			{
				setCheck(row, CheckState_t::Checked, false, true);
			}
		}
	}
}

//------------------------------------------------------------------------------
bool
FileTreeView::on_test_expand_row(const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path&)
{
	reload_from_filesystem(iter);
	return false;
}


//------------------------------------------------------------------------------
template <class TreeIter_t>
void
FileTreeView::fillChildren(const std::string & path, CheckState_t checkState, TreeIter_t & parentTreeRow)
{
	// todo: check if path still exists ?..
	std::vector< Glib::RefPtr<Gio::FileInfo> > fileInfoVector;
	collectChilden(path, fileInfoVector);

	Gtk::TreeIter itExistingItem = parentTreeRow.children().begin();
	for (auto && file_info: fileInfoVector)
	{
		if (itExistingItem)
		{
			Gtk::TreeRow treeRow = *itExistingItem;
			const std::string existingFileName = getFileName(treeRow);
			const std::string newFileName = file_info->get_name();

			// make sure that directories should go first
			int cmp = 0;
			if ( !treeRow.children().empty() == (file_info->get_file_type() == Gio::FILE_TYPE_DIRECTORY) )
			{
				cmp = existingFileName.compare(newFileName);
			} else
			{
				cmp = treeRow.children().empty() ? 1 : -1;
			}

			if (cmp > 0)
			{
				Gtk::TreeRow newRow = *m_store->insert(itExistingItem);
				fillTreeRow(file_info, path, newRow);
				if (checkState != CheckState_t::Mixed)
				{
					setCheck(newRow, checkState, true, false);
				}
			} else
			if (cmp < 0)
			{
				while ( (itExistingItem = m_store->erase(itExistingItem)) )
				{
					if (getFileName(itExistingItem) == newFileName)
					{
						++itExistingItem;
						break;
					}
				}
			} else
			{
				++itExistingItem;
			}
		} else
		{
			Gtk::TreeRow treeRow = *m_store->append(parentTreeRow.children());
			fillTreeRow(file_info, path, treeRow);
			if (checkState != CheckState_t::Mixed)
			{
				setCheck(treeRow, checkState, true, false);
			}
		}
	}

	while (itExistingItem)
	{
		itExistingItem = m_store->erase(itExistingItem);
	}
}


//------------------------------------------------------------------------------
void
FileTreeView::on_cell_toggled(const Glib::ustring & path)
{
	auto iter = m_store->get_iter(path);
	if (!iter) return;
	auto item = *iter;
	const CheckState_t newCheckState = (item[m_columns.checkState] == CheckState_t::Unchecked) ?
		CheckState_t::Checked :
		CheckState_t::Unchecked;
	setCheck(item, newCheckState, false, false);
}


//------------------------------------------------------------------------------
void
FileTreeView::collectChilden(const std::string & parentPath, std::vector<FolderTreeItem> & children)
{
	Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(parentPath);

	Glib::RefPtr<Gio::FileEnumerator> child_enumeration = file->enumerate_children("standard::name,standard::is-hidden");

	std::vector< Glib::RefPtr<Gio::FileInfo> > fileInfoVector;
	while (auto file_info = child_enumeration->next_file())
	{
		if (!file_info) continue;
		if (!m_showHiddenFiles && file_info->is_hidden()) continue;
		fileInfoVector.push_back(file_info);
	}

	constexpr auto lambdaSortFileInfo = [](const Glib::RefPtr<Gio::FileInfo> & l, const Glib::RefPtr<Gio::FileInfo> & r) {
		if (l->get_file_type() == r->get_file_type())
		{
			return l->get_name() < r->get_name();
		}
		if (l->get_file_type() == Gio::FILE_TYPE_DIRECTORY)
		{
			return true;
		}
		if (r->get_file_type() == Gio::FILE_TYPE_DIRECTORY)
		{
			return false;
		}
		return l->get_name() < r->get_name();
	};
	std::sort(fileInfoVector.begin(), fileInfoVector.end(), lambdaSortFileInfo);
	children.swap(fileInfoVector);
}


//------------------------------------------------------------------------------
FileTreeView::CheckState_t
FileTreeView::getCheck(const Gtk::TreeIter & iter) const
{
	if (!iter) return CheckState_t::Unchecked;
	return (*iter)[m_columns.checkState];
}


//------------------------------------------------------------------------------
bool
FileTreeView::setCheck(const Gtk::TreeIter & iter, CheckState_t checkState, bool noUp, bool noDown)
{
	if (!iter) return false;
	if (getCheck(iter) == checkState) return true;

	(*iter)[m_columns.checkState] = checkState;

	if (!noDown)
	{
		if (checkState != CheckState_t::Mixed)
		{
			for (Gtk::TreeIter kid : iter->children())
			{
				setCheck(kid, checkState, true, noDown);
			}
		}
	}

	if (!noUp)
	{
		auto && parent = iter->parent();
		if (parent)
		{
			bool all = true;

			for (auto && it : parent->children())
			{
				if (getCheck(it) != checkState)
				{
					all = false;
					break;
				}
			}

			if (all)
			{
				setCheck(parent, checkState, noUp, true);
			} else
			{
				setCheck(parent, CheckState_t::Mixed, noUp, true);
			}
		}
	}

	return true;
}


//------------------------------------------------------------------------------
void
FileTreeView::load(const std::string & path)
{
	if (m_basePath == path) return;
	m_basePath = path;
	m_store->clear();
	if (m_basePath.empty()) return;
	std::vector< Glib::RefPtr<Gio::FileInfo> > fileInfoVector;
	collectChilden(path, fileInfoVector);

	for (auto && file_info: fileInfoVector)
	{
		Gtk::TreeRow treeRow = *m_store->append();
		fillTreeRow(file_info, path, treeRow);
	}

	if (get_realized()) scroll_to_point(0, 0);
}


//------------------------------------------------------------------------------
void
FileTreeView::fillTreeRow(
		const Glib::RefPtr<Gio::FileInfo> & fileInfo,
		const std::string & path,
		Gtk::TreeRow & treeRow)
{
	if (!fileInfo) return;
	const std::string & aFilePath = Glib::build_filename(path, fileInfo->get_name());

	treeRow[m_columns.fake] = false;
	treeRow[m_columns.fullPath] = aFilePath;
	treeRow[m_columns.checkState] = CheckState_t::Unchecked;

	if (fileInfo->get_file_type() == Gio::FILE_TYPE_DIRECTORY)
	{
		Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(aFilePath);
		try
		{
			if (file && file->enumerate_children())
			{
				auto t = m_store->append(treeRow->children());
				(*t)[m_columns.fake] = true;
			}
		} catch (const Glib::Exception &ex)
		{
			// disable checkbox
			g_warning("%s", ex.what().c_str());
		}
	}
}


//------------------------------------------------------------------------------
void
FileTreeView::selectAll()
{
	for (auto && iter : m_store->children())
	{
		setCheck(*iter, CheckState_t::Checked, true, false);
	}
}


//------------------------------------------------------------------------------
void
FileTreeView::clearSelection()
{
	for (auto && iter : m_store->children())
	{
		setCheck(*iter, CheckState_t::Unchecked, true, false);
	}
}


//------------------------------------------------------------------------------
void
FileTreeView::refresh()
{
	if (m_basePath.empty()) return;
	freeze_notify();
	try
	{
		fillChildren(m_basePath, CheckState_t::Mixed, (*m_store.operator->()));
		for (auto && kid : m_store->children())
		{
			refresh_children(kid);
		}
	} catch (...)
	{
		g_warning("Error during FileTreeView::refresh");
	}
	thaw_notify();
}


//------------------------------------------------------------------------------
void
FileTreeView::set_show_hidden(bool value)
{
	if (m_showHiddenFiles != value)
	{
		m_showHiddenFiles = value;
		refresh();
	}
}


}}
